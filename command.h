#ifndef COMMAND_H
#define COMMAND_H
#include "cjson.h"
#include <string>
#include <thread>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include "cmonitoring.h"
#include "clog.h"

using namespace std;
/////////////////////////////////////////////////
/// based class of command
/// Command - базовый класс для команд
/// абстрактный интерфейс для всего набора команд
/////////////////////////////////////////////////
class Command {
public:
    //! Command constructor.
     /*!
      \param strAdrr адрес web-сервера ядра системы мониторинга.
      \param j тело запроса с данными в формате JSON
     */
    Command(string strAdrr, CJSON j, CLog* ptrLog = NULL);
    ~Command();
    //! общий интерфейс для методов запуска команд
    virtual void runCommand();
    //! получит количесиво потоков на данный момент
    //! нужно для отладки
    size_t getCountThread();
    int getCount();
protected:
    //! объект с данными в формате JSON
    CJSON _json;
    //! контейнер с объектами потоков мониторинга
    static map<string, shared_ptr<CMonitoring>> _mMon;
    //! целочисленный идентификатор объектов топологий
    int _idTopology;
    static size_t _id;
    //! строка с адресом web-сервера ядра мониторинга
    string _strAdrr;
    //! идентификатор объектов топологий в формате UUID
    string _idTopologyUUID;
    //! указатель на объект лога
    CLog* _ptrLog;
    static int count;
};

//! производный классы на каждую команду в отдельности


/////////////////////////////////////////////////
/////class of command creating topology - create
/////////////////////////////////////////////////
class CreateMonitoring : public Command
{
public:
    CreateMonitoring(string strAdrr, CJSON j, vector<string>& vLocalIp, unsigned int pause, CLog* ptrLog):
        Command(strAdrr, j, ptrLog),
        _vLocalIp(vLocalIp),
        _pause(pause)
    {

    }
    void runCommand();
private:
    vector<string>& _vLocalIp;
    unsigned int _pause;
};
/////////////////////////////////////////////////////////////
/// \brief The CGetTopology class of command getting topology
/////////////////////////////////////////////////////////////
class CGetTopology : public Command
{
public:
    CGetTopology(string strAdrr, CJSON j, CLog* ptrLog):Command(strAdrr, j, ptrLog){}
    void runCommand();
};
//////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The CStartMonitoring class used for command of begining monitoring topology of network
//////////////////////////////////////////////////////////////////////////////////////////////////
class CStartMonitoring : public Command
{
public:
    CStartMonitoring(string strAdrr, CJSON j, CLog* ptrLog):Command(strAdrr, j, ptrLog){}
    void runCommand();
};

class CStopMonitoring : public Command
{
public:
    CStopMonitoring(string strAdrr, CJSON j, CLog* ptrLog):Command(strAdrr, j, ptrLog){}
    void runCommand();
};

class CAddMonitoring : public Command
{
public:
    CAddMonitoring(string strAdrr, CJSON j, CLog* ptrLog):Command(strAdrr, j, ptrLog){}
    void runCommand();
};

class CDeleteMonitoring : public Command
{
public:
    CDeleteMonitoring(string strAdrr, CJSON j, CLog* ptrLog):Command(strAdrr, j, ptrLog) {}
    void runCommand();
};

class CKillMonitoring : public Command
{
public:
    CKillMonitoring(string strAddr, CJSON j, CLog* ptrLog):Command(strAddr, j, ptrLog) {}
    void runCommand();
};

//////////////////////////////////////////////////////////////////////////////////
/// \brief The CommandFactory class is abstruct factory of creation command object
//////////////////////////////////////////////////////////////////////////////////
class CommandFactory {
public:
    CommandFactory( CLog *ptrLog, unsigned int pause);
    void setStrAdrr(string strAdrr);
    //! заполнение векстора списка возможных вариантов ip-адресов
    void setLocalIp(vector<string> vLocalIp);
    shared_ptr<Command> createCommandCreate(CJSON j); //!< that is creating command "create"
    shared_ptr<Command> createCommandGetTopology(CJSON j); //!< that is creating command "get_topology"
    shared_ptr<Command> createCommandStartMon(CJSON j); //!< that is creating command "start_mon"
    shared_ptr<Command> createCommandStopMon(CJSON j); //!< that is creating command "stop_mon"
    shared_ptr<Command> createCommandAdd(CJSON j); //!< that is creating command "add"
    shared_ptr<Command> createCommandDelete(CJSON j); //!< that is creating command "delete"
    shared_ptr<Command> createCommandKill(CJSON j); //!< that is creating command "kill"
    shared_ptr<Command> createCommand(CJSON j); //!< that is creating for any command
    shared_ptr<Command> createEmpty(CJSON j); //!< the method is suppress
private:
    typedef shared_ptr<Command> (CommandFactory::*ptrMethod_t)(CJSON j); //!< type of pointer method of the class
    map<string, ptrMethod_t> _mCommands; //!< map for saving methods of creating all commands
    string _strAdrr;
    vector<string> _vLocalIp;
    CLog* _ptrLog;
    //! пауза для сканирования мониторинга
    unsigned int _pause;
};

#endif
