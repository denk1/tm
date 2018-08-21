#include "cmonitoring.h"

CMonitoring::CMonitoring(string strAdrr, CJSON j, vector<string>& vLocalIp, unsigned int pause, CLog *ptrLog):
    _fssAPI(strAdrr, _strToken, j, ptrLog),
    _isContinue(true),
    _isKill(false),
    _hostId(0),
    _isGet(false),
    _ptrLog(ptrLog),
    _nmap(ptrLog),
    _vLocalIp(vLocalIp),
    xml(ptrLog),
    _j(j),
    _pause(pause),
    _isMonitoring(false)
{

}

CMonitoring::~CMonitoring()
{

}

int CMonitoring::runGetTopology( bool lock, bool isSingle)
{
    bool isGet = _isGet;
    // очистка локального контейнера со списком ip-адресов локальными ip-адресами
    _sHosts.clear();
    // заполнение локального контейнера со списком ip-адресов локальными ip-адресами
    for(auto& strIp:_vLocalIp)
    {
        _sListIp.insert(strIp);
    }
    // заполнение контейнера со списком хостов
    _rwmutex.lock();
    for(auto it : _sListIp)
    {
        _sHosts.insert(it);
    }
    _rwmutex.unlock();
    // передача списка хостов объекту для работы системной командой nmap
    _nmap.setListIp(_sHosts);
    // запуск на выполнение системной команды nmap

    _nmap.RunNMap(_strResult, _j);
    // чтения вывода с результатом работы команды nmap в формате xml
    string strFileResultXml = ".xml";
    //////
    xml.ReadXML(_strResult);
    // получение контейнера со списком связей
    VectorLinkHosts& vLinkHost = xml.getLinkHost();
    // сортировка массива текущих связей
    sort(vLinkHost.begin(), vLinkHost.end());
    // массив для хранения разности
    VectorLinkHosts vDiffLinks;
    // установка токена соединения с backend-сервером
    _fssAPI.setToken(_strToken);
    // нахождение исчезнувших связей из разности предыдущего и текущего массивов со связями
    set_difference(_vPrevLinkHost.begin(), _vPrevLinkHost.end(), vLinkHost.begin(), vLinkHost.end(),
                        inserter( vDiffLinks,  vDiffLinks.begin()));
    if(vDiffLinks.size() > 0)
    {
        realiseJSON(vDiffLinks, false, &CMonitoring::delLinks);
        vDiffLinks.clear();
    }
    else
    {
        _ptrLog->writeLineLevel1("monitoring", "old links are not detected");
    }

    if(isGet)
    {
        realiseJSON(vLinkHost, true, &CMonitoring::postLinks);
        _isGet = false;
        _isMonitoring = true;
    }
    else
    {
        // нахождение новых связей из разности текущего и предыдущего массивов со связями
        set_difference(vLinkHost.begin(), vLinkHost.end(), _vPrevLinkHost.begin(), _vPrevLinkHost.end(),
                            inserter( vDiffLinks,  vDiffLinks.begin()));
        // заполнение json-объекта новыми связями
        if(vDiffLinks.size() > 0)
        {
            realiseJSON(vDiffLinks, false, &CMonitoring::postLinks);
        }
        else
        {
            _ptrLog->writeLineLevel1("monitoring", "new links are not detected");
        }
    }
    _vPrevLinkHost.clear();
    _vPrevLinkHost.insert(_vPrevLinkHost.begin(), vLinkHost.begin(), vLinkHost.end());
//    string strUUID = _jTest["id_topology"];
//    ostringstream idXml;
//    idXml << _xmlID;
//    _f.open(strUUID + "-" + idXml.str() + ".xml", ios_base::trunc);
//    _f << _strResult;
//    _f.close();
    //очистка контейнера ссылок
    vLinkHost.clear();
    return 0;
}

int CMonitoring::runMonitoring( bool lock, bool isSingle)
{
    // оставнока потока мониторинга
    _lock = lock;
    // флаг режима мониторига, если false - то получен единоразовый запрос на информацию о топологии
    _isSingle = isSingle;
    // проверка: продолжать ли мониторинг
    while(_isContinue)
    {
        while (_lock);
        if(_sListIp.size() > 1)
        {
            if(!_isKill)
                runGetTopology( lock, _isSingle);
        }
        else
        {
            _ptrLog->writeLineLevel1("Monitoring", "list of ip-address is empty");
            _lock = true;
        }

        if(_isSingle)
        {
            lock = true;
        }
        if(_isMonitoring)
            this_thread::sleep_for(chrono::seconds(_pause));
    }

}

bool CMonitoring::delLinks(CJSON &jLinks)
{
    _fssAPI.delTopologyLinks(jLinks);
}

bool CMonitoring::postLinks(CJSON &jLinks)
{
    _fssAPI.postTopologyLinks(jLinks);
}

void CMonitoring::realiseJSON(VectorLinkHosts &vLinks, bool isGet, bool (CMonitoring::*ptrPostMethod)(CJSON&))
{
    // json-объект для отправки новых связей
    json_object* jLinks = json_object_new_array();
    CJSON cjsonPost;
    json_object* jsonBody = cjsonPost.getPtrJsonObj();
    for(auto& link: vLinks)
    {
        json_object * jLink = json_object_new_object();
        json_object *jStrFromIp = json_object_new_string(link.strIp1.c_str());
        json_object *jStrToIp = json_object_new_string(link.strIp2.c_str());
        json_object_object_add(jLink, "from_ip", jStrFromIp);
        json_object_object_add(jLink, "link_type", json_object_new_string("edb4614e-5e99-4ddc-ad2e-3ee89a88a161"));
        json_object_object_add(jLink, "to_ip", jStrToIp);
        json_object_array_add( jLinks, jLink);
    }
    if(isGet)
    {
        json_object* bJsonObj = json_object_new_boolean(1);
        json_object_object_add(jsonBody, "get_topology", bJsonObj);
    }
    json_object_object_add(jsonBody, "links", jLinks);
    (this->*ptrPostMethod)(cjsonPost);
}


void CMonitoring::getTopology()
{
    _isGet = true;
}

void CMonitoring::createMonitoring()
{
     // сохранения нового потока мониторинга в контейнера
    _listThrd.push_back(thread(&CMonitoring::runMonitoring, this, true, false));
}

void CMonitoring::startMonitoring()
{
    _lock = false;
}

void CMonitoring::stopMonitoring()
{
    _lock = true;
}

void CMonitoring::addMonitoring()
{
    if(_sListIp.size() > 1)
    {
        _lock = false;
    }
}

void CMonitoring::deleteMonitoring()
{
    // TO DO
}

void CMonitoring::killMonitoring()
{
    _isContinue = false;
    _lock = false;
    _isKill = true;
    _listThrd.back().join();
}

void CMonitoring::setListIp(CJSON& j)
{
    // получения списка ip-адресов из входных данных
    json_object* arrIps = j.getValueJsonArray("ip_list");
    int array_size = json_object_array_length(arrIps);
    _rwmutex.lock();
    for(int i = array_size-1; i>=0; --i)
    {
        json_object *jsn_temp = json_object_array_get_idx(arrIps, i);
        const char* cStrHost = json_object_get_string(jsn_temp);
        _sListIp.insert(cStrHost);
    }
    _rwmutex.unlock();
}

void CMonitoring::delListIp(CJSON &j)
{
    // удаление ip-адресов из списка сканирования
    json_object* arrIps = j.getValueJsonArray("ip_list");
    int array_size = json_object_array_length(arrIps);
    _rwmutex.lock();
    for(size_t i = array_size-1; i>=0; --i)
    {
        json_object *jsn_temp = json_object_array_get_idx(arrIps, i);
        const char* cStrHost = json_object_get_string(jsn_temp);
        _ptrLog->writeLineLevel1(cStrHost, _sListIp.size());
        if(_sListIp.end()== _sListIp.find(cStrHost))
        {
            _ptrLog->writeLineLevel1("deleting ip", "This ip-adress isn't found in list of ip-adresses");
        }
        else
        {
            _sListIp.erase(cStrHost);
            _ptrLog->writeLineLevel1("deleting ip", "This ip-adress is deleted from ip list are success");
        }
    }
    _rwmutex.unlock();
}

string CMonitoring::_strToken;

void CMonitoring::setTokenKey(string strTkn)
{
    _strToken = strTkn;
}
