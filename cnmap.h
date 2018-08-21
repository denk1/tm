#ifndef CNMAP_H
#define CNMAP_H
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <set>
#include "clog.h"
#include "cjson.h"

using namespace std;

//!  CNmap - класс инкапсулирующий функционал системной команды nmap, для большей информации см. https://nmap.org/
/*!
  получает данные и запускает nmap на выполнение
*/

class CNmap
{
public:
    //! конструктор
    CNmap(CLog* ptrLog);
    //! устанавливает список ip-адресов
    void setListIp(set<string> &sIp );
    //! запускает на выполнение команду nmap
    void RunNMap(string& strResult, CJSON j);

private:
    //! внутренний запуск на выполнение и формирование строки с результатом
    void execNmap(const char* cmd, string& result, CJSON j);
    //! строка, хранящая параметры
    std::string _strParam;
    //! полная строка команды nmap
    std::string _strCommand, _strCommandFinal;
    //! указатель на объект лога
    CLog* _ptrLog;
};

#endif
