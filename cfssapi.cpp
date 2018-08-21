#include "cfssapi.h"
#include <sstream>

CFssapi::CFssapi(string strAddr, string strTkn, CJSON j, CLog *ptrLog):CAddrAPI(strAddr, strTkn, ptrLog), _strTokenKey(strTkn)
{
    setServerUrl(j);
}

CFssapi::~CFssapi()
{

}

void CFssapi::setServerUrl(CJSON &j)
{
    //Commands
    if(j.dump()!="{ }")
        _strUUID = j.getValueString("id_topology");
    //getting links list of topologies
    // формирование команды получения списка всех топологий, обрабатываемых в данный момент на сервере
    _mCommand["GetListTopologies"].url = _strBeginUrl + _strServerIpAddr + "/api/v1/topologies";
    _mCommand["GetListTopologies"].method = Method::GET;
    //getting info about of the topology
    _mCommand["GetToplogyInfo"].url = _strBeginUrl + _strServerIpAddr + "/api/v1/topologies/" + _strUUID;
    _mCommand["GetToplogyInfo"].method = Method::GET;
    //getting list of links from the topology
    _mCommand["GetTopologyLinks"].url = _strBeginUrl + _strServerIpAddr + "/api/v1/topologies/" + _strUUID + "/links";
    _mCommand["GetTopologyLinks"].method = Method::GET;
    //getting ip list
    _mCommand["GetTopologyIps"].method = Method::GET;
    //saving list of links
    _mCommand["PostTopologyLinks"].url =  _strBeginUrl + _strServerIpAddr + "/api/v1/topologies/" + _strUUID + "/links";
    _mCommand["PostTopologyLinks"].method = Method::POST;
    //test GET request
    _mCommand["GetTest"].url = "http://81.177.160.249:3800/api/v1/auth";
    _mCommand["GetTest"].method = Method::POST;
    // deleting links from topologies
   _mCommand["DelTopologyLinks"].url = _strBeginUrl + _strServerIpAddr + "/api/v1/topologies/" + _strUUID + "/links";
   _mCommand["DelTopologyLinks"].method = Method::DELETE;
}

bool CFssapi::postTopologyLinks(CJSON jsonLinks)
{
    _mCommand["PostTopologyLinks"].j = jsonLinks;

    postResponse(_mCommand["PostTopologyLinks"]);
}

bool CFssapi::delTopologyLinks(CJSON jsonLinks)
{
    _mCommand["DelTopologyLinks"].j = jsonLinks;

    postResponse(_mCommand["DelTopologyLinks"]);
}

bool CFssapi::getTopologies()
{
    postResponse(_mCommand["GetListTopologies"]);
}

bool CFssapi::getTopologyIps(string strUUID)
{
    _mCommand["GetTopologyIps"].url = _strBeginUrl + _strServerIpAddr + "/api/v1/topologies/" + strUUID + "/ips";
    _strUUID = strUUID;
    postResponse(_mCommand["GetTopologyIps"]);
}

void CAddrAPI::setOptionCurl(CommandAPI &cmd)
{
    _s.ptr = NULL;
    _strJsonObj = cmd.j.dump();
    const char* url = cmd.url.c_str();

    if(_ptrCurl) {
        init_string(&_s);
        const char* chMethod = _methods[cmd.method];
        if(cmd.method == Method::GET)
        {
            curl_easy_setopt(_ptrCurl, CURLOPT_HTTPGET, 1L);
        }
        else if(cmd.method == Method::POST)
        {
            curl_easy_setopt(_ptrCurl, CURLOPT_POSTFIELDS, _strJsonObj.c_str());
        }
        else
        {
            curl_easy_setopt(_ptrCurl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(_ptrCurl, CURLOPT_POSTFIELDS, _strJsonObj.c_str());
        }


        curl_easy_setopt(_ptrCurl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(_ptrCurl, CURLOPT_WRITEFUNCTION, Curl::writefunc);
        curl_easy_setopt(_ptrCurl, CURLOPT_WRITEDATA, &_s);
        //curl_easy_setopt(_ptrCurl, CURLOPT_POSTFIELDSIZE, _strJsonObj.size());
        //curl_easy_setopt(_ptrCurl, CURLOPT_POST, 1);
        if(!_strToken.empty())
        {
            _strHeader = "x-auth:" + _strToken;
            //cout << "token:" << _strToken << endl;
            _ptrHeaders = curl_slist_append(_ptrHeaders, _strHeader.c_str());
        }
        if(_ptrLog)
            _ptrLog->writeLineLevel1("POST url API: ", url);
        _ptrHeaders = curl_slist_append(_ptrHeaders, "Accept: application/json");
        _ptrHeaders = curl_slist_append(_ptrHeaders, "Content-Type: application/json");
        _ptrHeaders = curl_slist_append(_ptrHeaders, "charsets: utf-8");

        curl_easy_setopt(_ptrCurl, CURLOPT_HTTPHEADER, _ptrHeaders);
        if(_ptrLog)
            _ptrLog->writeLineLevel1("POST json API: ", "UUID of topology:" + _strUUID + " " + _strJsonObj);
        curl_easy_setopt(_ptrCurl, CURLOPT_URL, url);
    }
}


CAuthApi::CAuthApi(string strAdrr):CAddrAPI(strAdrr)
{
    _commandAPI.method = Method::POST;
    //authorization
    _commandAPI.url = "/api/v1/signin";
}

CAddrAPI::CAddrAPI(string strAddr, string strToken, CLog *ptrLog):Curl(ptrLog),
    _strServerIpAddr(strAddr),
    _strBeginUrl("http://"),
    _strToken(strToken)
{

}

void CAddrAPI::postResponse(CommandAPI& cmd)
{

    //Curl::initCurl();
    setOptionCurl(cmd);
    Curl::postResponse();
    _strResultResponse = Curl::getResultJSON().ptr;
    Curl::freeCurlSession();
    //Curl::freeCurl();
}

void CAddrAPI::setToken(string strToken)
{
    _strToken = strToken;
}

string CAddrAPI::getResultJSON()
{
    return _strResultResponse;
}

///////////////////////////////////////////////////
/// \brief CFssapi::logInToServer is authorized to the server
/// \return true in case succes authorized
///
bool CAuthApi::logInToServer(string strUserName, string strPsw)
{
    CJSON jAuth;
    // сборка строки запроса
    _commandAPI.url = _strBeginUrl + _strServerIpAddr + _commandAPI.url;
    // заполнение контейнера c данными в формате JSON
    json_object_object_add(jAuth.getPtrJsonObj(), "username",  json_object_new_string(strUserName.c_str()));
    json_object_object_add(jAuth.getPtrJsonObj(), "password",  json_object_new_string(strPsw.c_str()));
    _commandAPI.j = jAuth;
    // отправка запроса на web-сервер системы мониторинга
    postResponse(_commandAPI);
}
