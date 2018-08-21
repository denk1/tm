#ifndef CFSSAPI_H
#define CFSSAPI_H
#include <string>
#include <map>
#include "curl.h"
#include "clog.h"
#include "cjson.h"


using namespace std;

///////////////////////////////////////////////////////////////////
/// \brief The Method enum is kind of the methods HTTP request
/////////////////////////////////////////////////////////////////
/// типы запроса на web-сервер
enum Method
{
    GET,
    POST,
    DELETE
};
//! структура команды
struct CommandAPI
{
    string url;
    Method method;
    CJSON j;
};

// ////////////////////////////////
/// \brief The CAddrAPI class is authrization
/// That is base class for the working fss-API


class CAddrAPI :  public Curl
{
public:
    CAddrAPI(string strAddr, string strToken = "", CLog* ptrLog = NULL);
    void postResponse(CommandAPI &cmd);
    void setToken(string strToken);
    string getResultJSON();
protected:
    //<! метод установки требуемых параметров библиотеки cURL, необходимых для отправки запросов
    void setOptionCurl(CommandAPI& cmd);
    //<! строка хранения ip-адреса web-сервера ядра системы мониторинга
    const string _strServerIpAddr;
    //<! cтрока типа протокола запроса на web-сервер ядра системы мониторинга
    const string _strBeginUrl;
    //<! строка данных запроса в формате JSON
    string _strJsonObj;
    //<! метод запроса
    const char* _methods[3] = {"GET", "POST", "DELETE"};
    string _strUUID;
private:
    string _strResultResponse;
    string _strHeader;
    string _strToken;
};

//class of authotification API
// класс аутентификации с web-сервером ядра системы мониторинга
//
class CAuthApi : public CAddrAPI
{
public:
    CAuthApi(string strAdrr);
    bool logInToServer(string strUserName, string strPsw); //!< метод аутентификации, param: strUserName - имя пользователя, strPsw - пароль
private:
    //<! объект команды получаемой сервисом мониторига
    CommandAPI _commandAPI;
};

class CFssapi : public CAddrAPI
{
public:
    CFssapi(string strAddr,  string strTkn, CJSON j = CJSON(),CLog *ptrLog = NULL);
    ~CFssapi();
    //! инициализация строки запроса для отправки данных ядру системы мониторинга
    /*!
     \param j - тело запроса в формате JSON
     */
    void setServerUrl(CJSON& j);
    //! отправка результатов ядру системы мониторинга
    /*!
     \param jsonLinks - тело запроса в формате JSON
     */
    bool postTopologyLinks(CJSON jsonLinks);
    /*!
      \param jsonLinks - тело запроса в формате JSON
      */
    bool delTopologyLinks(CJSON jsonLinks);
    //! получение списка активных топологий
    bool getTopologies();
    //! получение списка связей активных топологий
    bool getTopologyIps(string strUUID);


private:
    //! контейнер с командами API сервера мониторинга
    map<string, CommandAPI> _mCommand;
    //! ключи шифрования системы безопасности системы мониторинга
    const string _strTokenKey;
    size_t _idTop;

    bool _bAuthUser = false;

    const char* _chPathAuth = "http://81.177.160.249:3800/api/v1/auth";
    const char* _chPathTopologies = "http://81.177.160.249:3800/api/v1/topologies";
    string _strToken;
};

#endif
