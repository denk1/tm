#ifndef CRUN_H
#define CRUN_H

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <memory>
#include <vector>
#include <exception>
#include <list>
#include "cmonitoring.h"
#include "cthread.h"
#include "cjson.h"
#include "command.h"
#include "cfssapi.h"
#include "clog.h"

using namespace std;
//!  SAuth - структура для храниения данных аутентификации на web-сервера ядра системы мониторинга

struct SAuth
{
    //! имя пользователя
    string strUserName;
    //! пароль
    string strPsw;
};

//!  CRun - класс объекта сервиса мониторинга
/*!
  содержит цикл главного потока
*/

class CRun
{
public:
    //! конструктор
    CRun(string strAdrr, CLog* ptrLog, unsigned int pause);
    //! устанвливает паузу
    void setTimeout(int t);
    //! устанавливает данные для аутентификации
    void setAuthData(string strUser, string strPwd);
    //! запускает выполнение
    int start();
private:
    //! производит аутентификацию
    //! /return вовращает результат верефикации аккаунта пользователя
    bool authentication();
    //! получения списка "локальных" ip-адресов
    int getLocalIp();
    //! время задержки выполнения главного цикла
    int _t;
    //! объект потока
    CThread thrd;
    //! контейнер с потоками объектов мониторинга
    vector<auto_ptr<CMonitoring>> vMon;
    //! абстрактная фабрика объектов команд
    CommandFactory _commandFactory;
    //! объект функционала аутентификации
    CAuthApi _fssAuthAPI;
    //! метод для аутентификации
    bool authentification();
    //! данные для аутентификации
    SAuth _auth;
    //! строка адреса backend-сервера
    string _strUrlBackend;
    //! вектор локальных ip-адресов
    vector<string> _vLocalIp;
    //! указатель на объект лога
    CLog* _ptrLog;
    //! строка с токеном доступа к backend-api
    string _strToken;
};

#endif
