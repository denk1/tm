#ifndef CMONITORING_H
#define CMONITORING_H
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <sstream>
#include <memory>
#include <fstream>
#include <mutex>
#include "curl.h"
#include "cnmap.h"
#include "cxml.h"
#include "cthread.h"
#include "cfssapi.h"
#include "clog.h"
#include "cjson.h"



using namespace std;

//!  CMonitoring - класс объектов мониторинга, которым соответсвует отдельный поток
/*!
  инициализируется отдельно для каждого запроса
*/


class CMonitoring : public CThread
{
public:
    //! CMonitoring constructor.
     /*!
      \param strAdrr адрес web-сервера ядра системы мониторинга.
      \param vL указатель на контейнер с ip-адресами для мониторинга.
     */
    CMonitoring(string strAdrr, CJSON j, vector<string>& vLocalIp, unsigned int pause, CLog *ptrLog);
    ~CMonitoring();
    //! метод запуска команды мгновенного получения топологии
    void getTopology();
    //! метод создания потока для объекта мониторинга
    void createMonitoring();
    //! метод начала монитооринга
    void startMonitoring();
    //! остановка мониторинга
    void stopMonitoring();
    //! добавление списка адресов монитооинга
    void addMonitoring();
    //! удаление из списка адресов мониторинга
    void deleteMonitoring();
    //! удаления потока мониторинга
    void killMonitoring();
    //! инициализация списка адресов мониторинга
    void setListIp(CJSON& j);
    //! удаление списка ip-адресов
    void delListIp(CJSON& j);
    //! установка ключа шифрования системы безопасности ядра системы мониторинга
    static void setTokenKey(string strTkn);
    //! контейнер для хранения списка ip-адресов
    set<string> _sListIp;
private:
    //! метод реализующий команды мгновенного получения топологии
    int runGetTopology(bool lock, bool isSingle);
    //! метод реализующий команду запуска ядра мониторинга
    int runMonitoring(bool lock, bool isSingle);
    //! создание объекта json со списком связей и отправка на backend
    void realiseJSON(VectorLinkHosts& vLinks, bool isGet, bool (CMonitoring::*ptrPostMethod)(CJSON&));
    //! удаление ссылок
    bool delLinks(CJSON& jLinks);
    //! добавление ссылок
    bool postLinks(CJSON& jLinks);

    //! индексированный контейнер списка ip-адресов
    map<unsigned int, string> mapIp;
    //! отсортированный контейнер для хранения пар ip-адрес - идентификационный номер(ID) контролируемого хоста
    map<string, unsigned int> mapIpIdInter;
    //! контейнер со списком хостов
    set<string> _sHosts;
    //! контейнер со списком связей
    VectorLinkHosts vLinkHosts;
    //! контейнер с объектами мониторинга
    vector<shared_ptr<CMonitoring>> vMon;
    //! объект для работы с системной командой nmap
    CNmap _nmap;
    //! объект для работы с данными в формате xml
    CXml xml;
    //! объект для работы с функционалом прикладного программного интерфейса ядрас системы мониторинга
    CFssapi _fssAPI;
    //! строка с ключом шифромания
    static string _strToken;
    //! строка с результатом запроса
    string _strResult;
    //! управляющие переменные для синхронизации потоков мониторинга
    bool _lock, _isSingle, _isContinue, _isKill;
    //! внутренний id для идентификации ip-адресов
    size_t _hostId;
    //! флаг команды get
    bool _isGet;
    //! указатель на объект лога
    CLog* _ptrLog;
    //! xml-файл для отладки
    //ofstream _f;
    //! контейнер со списком локальных ip-адресов
    vector<string>& _vLocalIp;
    //! предыдущий контейнер со связями
    VectorLinkHosts _vPrevLinkHost;
    //! id xml-файла
    mutex _rwmutex;
    // для теста
    CJSON _j;
    // пауза сканирования
    unsigned int _pause;
    // признак режима мониторинга с паузой
    bool _isMonitoring;
};
#endif
