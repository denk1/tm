#include "crun.h"

int CRun::start()
{
    _ptrLog->writeLineLevel1("authentification", "connection to service...");
    while(!authentication());
    _ptrLog->writeLineLevel1("authentification:", "authentication to fss server core is [OK]");
    //! заполнение вектора локальных ip-адресов
    getLocalIp();
    // установка локального ip-адреса
    _commandFactory.setLocalIp(_vLocalIp);
    // установка url backend-сервера
    _commandFactory.setStrAdrr(_strUrlBackend);
    // объявление объекта для получения данных об уже имеющихся сетевых топологий для возобновления их мониторинга
    CFssapi fssapi(_strUrlBackend, _strToken);
    // отправка запроса на получение текущей информации о топологиях
    fssapi.getTopologies();
    // получение результата запроса
    string strTopologiesJSON = fssapi.getResultJSON();
    // создание JSON-объекта из полученного ответа на запрос
    CJSON testCJson(strTopologiesJSON);
    json_object* lstJsonObj = testCJson.getValueJsonArray("data");
    int arraySize = json_object_array_length(lstJsonObj);
    for(int i = arraySize-1; i>=0; --i)
    {
        json_object* jsonObj = json_object_array_get_idx(lstJsonObj, i);
        CJSON cjsonObj, cjsonObjCreateCommand, cjsonObjStartCommand, cjsonObjAddCommand;
        cjsonObj.setPtrJsonObj(jsonObj);
        // инициализация объекта команды создания новой топологии
        json_object* jObjCreateCommand = cjsonObjCreateCommand.getPtrJsonObj();
        // инициализация объекта команды старта мониторинга
        json_object* jObjStartCommand = cjsonObjStartCommand.getPtrJsonObj();
        // инициализация объекта команды добавления списка новых ip-адресов для мониторинга
        json_object* jObjAddCommand = cjsonObjAddCommand.getPtrJsonObj();
        // получение id текущей топлогиии
        const char* strIdTop = cjsonObj.getValueString("id");
        // заполнение данными объекта команды создания новой топологии
        json_object* jObjTopId = json_object_new_string(strIdTop);
        json_object* jObjStrCreate = json_object_new_string("create");
        json_object_object_add(jObjCreateCommand, "action", jObjStrCreate);
        json_object_object_add(jObjCreateCommand, "id_topology", jObjTopId);
        // заполнение данными объекта старта мониторинга
        json_object* jObjStrStart = json_object_new_string("start_mon");
        json_object_object_add(jObjStartCommand, "action", jObjStrStart);
        json_object_object_add(jObjStartCommand, "id_topology", json_object_get(jObjTopId));
        // заполнение данными объекта команды для добавления новых ip-адресов
        json_object* jObjStrAdd = json_object_new_string("add");
        json_object_object_add(jObjAddCommand, "action", jObjStrAdd);
        json_object_object_add(jObjAddCommand, "id_topology", json_object_get(jObjTopId));
        fssapi.getTopologyIps(strIdTop);
        string strJsonListIp = fssapi.getResultJSON();
        CJSON cjsonIps(strJsonListIp);
        json_object* lstIp = cjsonIps.getValueJsonArray("data");
        json_object* jsonObjIps = json_object_new_array();
        int array_size = json_object_array_length(lstIp);
        for(int i = array_size-1; i>=0; --i)
        {
            json_object *jObjIp = json_object_array_get_idx(lstIp, i);
            CJSON cjsonIp;
            cjsonIp.setPtrJsonObj(jObjIp);
            const char* cStrIp = cjsonIp.getValueString("discovered_ip");
            json_object *jObjIpStr = json_object_new_string(cStrIp);
            json_object_array_add(jsonObjIps, jObjIpStr);
        }
        json_object_object_add(jObjAddCommand, "ip_list", jsonObjIps);
        shared_ptr<Command> ptrCommand = _commandFactory.createCommand(cjsonObjCreateCommand);
        ptrCommand->runCommand();
        ptrCommand.reset();
        ptrCommand = _commandFactory.createCommand(cjsonObjStartCommand);
        ptrCommand->runCommand();
        ptrCommand.reset();
        ptrCommand = _commandFactory.createCommand(cjsonObjAddCommand);
        ptrCommand->runCommand();
    }
    int iTest = 5;
    iTest *= 2;
    cout << iTest;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief CRun::authentication
/// \return result of authentification to the fss server core
///

bool CRun::authentication()
{
    return authentification();
}

int CRun::getLocalIp()
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);
    _ptrLog->writeLineLevel1("finder local ip", "search local ip-adress of service");
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            _ptrLog->writeLineLevel1(ifa->ifa_name, addressBuffer);
            string strLocal = addressBuffer;
            if(strLocal.substr(0, 3) != "127")
                _vLocalIp.push_back(addressBuffer);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            //printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
            _ptrLog->writeLineLevel1(ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return 0;
}

CRun::CRun(string strAdrr, CLog *ptrLog, unsigned int pause):_t(0),
    _fssAuthAPI(strAdrr),
    _strUrlBackend(strAdrr),
    _commandFactory(ptrLog, pause),
    _ptrLog(ptrLog)
{

}

void CRun::setTimeout(int t)
{
    _t = t;
}

void CRun::setAuthData(string strUser, string strPwd)
{
    _auth.strUserName = strUser;
    _auth.strPsw = strPwd;
}

bool CRun::authentification()
{
    _fssAuthAPI.logInToServer(_auth.strUserName, _auth.strPsw);
    if(_fssAuthAPI.is200ResponseCode())
    {
        CJSON cjson(_fssAuthAPI.getResultJSON());
        _strToken = cjson.getValueString("data");
        CMonitoring::setTokenKey(_strToken);
        return true;
    }
    else
        return false;
}
