#ifndef CONF_H
#define CONF_H

#include <fstream>
#include <map>
#include <string>
using namespace std;

//!  Conf - класс для работы с конфигурационным файлом сервиса мониторинга
/*!
  topology-monitor.conf - имя файла, содержащего информацию
*/


class Conf
{
public:
    //! конструкторы класса
    Conf();
    Conf(const char* chPathFile);
    //! получение конфигурационной информации из файла
    void getConfig();
    //! получения значение отдельного параметра по его имени
    const char *getValueByName(const char* name);
private:
    const string _strNameFileConf;
    const string _strPathFileConf;
    const string _strFileConf;
    // поток вывода в файл
    ifstream _file;
    // контейнер со зачениями параметров
    map<string, string> _mapConfVar;
};
#endif
