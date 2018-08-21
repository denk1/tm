#include "cnmap.h"
#include <stdlib.h>
#include <stdio.h>

CNmap::CNmap(CLog *ptrLog):_strCommand("nmap  -min-parallelism 500 --min-hostgroup 500 -sP -n --traceroute "), _ptrLog(ptrLog)
{

}

void CNmap::setListIp(set<string> &sIp)
{
    //флаг localhost
    bool flagLocalhost = false;
    _strCommandFinal = _strCommand;
    // очистка строки параметров
    _strParam.clear();
    for(set<string>::iterator it = sIp.begin(); it!= sIp.end(); ++it)
    {
        // проверяем ip-адрес локального хоста
        if(*it != "127.0.0.1")
        {
            _strParam += *it + " ";
        }
        else
        {
            flagLocalhost = true;
        }
    }
   //формируем строку команды nmap
    _strCommandFinal += _strParam;

    if(flagLocalhost)
    {
        _strCommandFinal += "127.0.0.1 ";
    }
    // добавляем к строке флаги вызова
    _strCommandFinal += "-oX ";
}


void CNmap::RunNMap(string &strResult, CJSON j)
{
    //очистка ссылки на входящую строку
    strResult.clear();
    // устанавливаем параметр для вывода результата в формате xml в консоль
    _strCommandFinal += "-";
    // запуск внутренней системной команды
    // nmap должна быть устаовленная на локальной машине!
    // отправка в лог команты nmap
    execNmap(_strCommandFinal.c_str(), strResult, j);
}

void CNmap::execNmap(const char *cmd, string &result, CJSON j)
{

    // буфер чтения размеров 128 байт
    std::array<char, 128> buffer;
    // запуск на выполнение
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    // проверка запуска
    if (!pipe) throw std::runtime_error("popen() failed!");
    // формирование строки с результатом
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
        {
            result += buffer.data();
        }
    }
}
