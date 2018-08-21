#include "conf.h"
#include <sstream>
#include <stdexcept>
#include <iostream>




Conf::Conf():_file("topology-monitor.conf")
{

}

Conf::Conf(const char *chPathFile):
    _strFileConf(chPathFile),
    _file(_strFileConf.c_str() ){}

void Conf::getConfig()
{
    string line;
    //проверка открытия файла конфигурации
    if(_file.is_open())
    {
    //! цикл чтения строк из файла
        while( getline(_file, line) )
        {
            istringstream is_line(line);
            string key;
            if( getline(is_line, key, '=') )
            {
                string value;
                if( getline(is_line, value) )
                _mapConfVar[key] = value;
            }
         }
    }
    else
    {
        cout << "error opening file of configuration" << endl;
        cout << "program has stopped" << endl;
        exit(1);
    }

}

const char *Conf::getValueByName(const char *name)
{
    try
    {
        // поиск параметра по индексу
        return _mapConfVar.at(string(name)).c_str();
    }
    catch(const out_of_range& oor)
    {
        std::cerr << "Out of Range error: " << oor.what() << '\n';
    }

}
