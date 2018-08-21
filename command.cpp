#include "command.h"
#include <algorithm>

size_t Command::_id = 0;

Command::Command(string strAdrr, CJSON j, CLog *ptrLog):
    _json(j),
    _strAdrr(strAdrr),
    _ptrLog(ptrLog)
{
    // заполнение идентификатора топологии
    _idTopologyUUID = j.getValueString("id_topology");
    _idTopology = _id++;
    count++;
}

Command::~Command()
{
    count--;
}

void Command::runCommand()
{
    _ptrLog->writeLineLevel1("command: unknown", "That is working suppress command");
}

size_t Command::getCountThread()
{
    return _mMon.size();
}

int Command::getCount()
{
    return count;
}

map<string, shared_ptr<CMonitoring>> Command::_mMon;
int Command::count = 0;

CommandFactory::CommandFactory(CLog *ptrLog, unsigned int pause):
    _ptrLog(ptrLog),
    _pause(pause)
{
    // инициализация контейнера ссылок на команды
    _mCommands["create"] = &CommandFactory::createCommandCreate;
    _mCommands["get_topology"] = &CommandFactory::createCommandGetTopology;
    _mCommands["start_mon"] = &CommandFactory::createCommandStartMon;
    _mCommands["stop_mon"] = &CommandFactory::createCommandStopMon;
    _mCommands["add"] = &CommandFactory::createCommandAdd;
    _mCommands["delete"] = &CommandFactory::createCommandDelete;
    _mCommands["kill"] = &CommandFactory::createCommandKill;
    // метод-"заглушка" на случай, когда приходит неизвестная команда
    _mCommands["empty"] = &CommandFactory::createEmpty;
    //ptrMethod_t ptrMethod = &CommandFactory::createCommand;
}

void CommandFactory::setStrAdrr(string strAdrr)
{
    _strAdrr = strAdrr;
}

void CommandFactory::setLocalIp(vector<string> vLocalIp)
{
    _vLocalIp.insert(_vLocalIp.begin(), vLocalIp.begin(), vLocalIp.end());
}


shared_ptr<Command> CommandFactory::createCommandCreate(CJSON j)
{
    //CreateMonitoring testCreateMonitoring(_strAdrr, j, _vLocalIp, _ptrLog);
    return shared_ptr<Command>(new CreateMonitoring(_strAdrr, j, _vLocalIp, _pause, _ptrLog));
}

shared_ptr<Command> CommandFactory::createCommandGetTopology(CJSON j)
{
    return shared_ptr<Command>(new CGetTopology(_strAdrr, j, _ptrLog));
}


shared_ptr<Command> CommandFactory::createCommandStartMon(CJSON j)
{
    return shared_ptr<Command>(new CStartMonitoring(_strAdrr, j, _ptrLog));
}


shared_ptr<Command> CommandFactory::createCommandStopMon(CJSON j)
{
    return shared_ptr<Command>(new CStopMonitoring(_strAdrr, j, _ptrLog));
}

shared_ptr<Command> CommandFactory::createCommandAdd(CJSON j)
{
    return shared_ptr<Command>(new CAddMonitoring(_strAdrr, j, _ptrLog));
}

shared_ptr<Command> CommandFactory::createCommandDelete(CJSON j)
{
    return shared_ptr<Command>(new CDeleteMonitoring(_strAdrr, j, _ptrLog));
}

shared_ptr<Command> CommandFactory::createCommandKill(CJSON j)
{
    return shared_ptr<Command>(new CKillMonitoring(_strAdrr, j, _ptrLog));
}

shared_ptr<Command> CommandFactory::createEmpty(CJSON j)
{
    return shared_ptr<Command>(new Command(_strAdrr, j, _ptrLog));
}

shared_ptr<Command> CommandFactory::createCommand(CJSON j)
{
    _ptrLog->writeLineLevel1( "method CommandFactory::createCommand(json j), JSON: ", j.dump());
    string strCommand = j.getValueString("action");
    try
    {
        return (this->*(_mCommands.at(strCommand)))(j);
    }
    catch(exception e)
    {
        return (this->*(_mCommands.at("empty")))(j);
    }
}

void CreateMonitoring::runCommand()
{
    _ptrLog->writeLineLevel1("Command: create", "That is working command of the creating topology");
    map<string, shared_ptr<CMonitoring>>::iterator it = _mMon.find(_idTopologyUUID);

    if(it == _mMon.end())
    {
        _mMon[_idTopologyUUID] = shared_ptr<CMonitoring>(new CMonitoring(_strAdrr, _json, _vLocalIp, _pause, _ptrLog));
        _mMon[_idTopologyUUID]->createMonitoring();
        string strMsg = "The topology with UUID=" + _idTopologyUUID + " is created!";
        _ptrLog->writeLineLevel1("Command: create", strMsg);

    }
    else
    {
        string strMsg = "Error of creating topopogy.The topology with UUID=" + _idTopologyUUID + " is created already!";
        _ptrLog->writeLineLevel1("Command: create", strMsg);
    }
}


void CGetTopology::runCommand()
{
    _ptrLog->writeLineLevel1("Command: get_topology", "That is working command of the getting topology");
    try
    {
        _mMon.at(_idTopologyUUID)->getTopology();
    }
    catch(exception e)
    {
        string strMsg = "Error of getting topology! The topology is UUID=" + _idTopologyUUID + " are not created!";
        _ptrLog->writeLineLevel1("Command: get_topology", strMsg);
    }
}


void CStartMonitoring::runCommand()
{
    _ptrLog->writeLineLevel1("Command: start_mon", "That is working command of the START monitoring");
    try
    {
        //! флаг старта мониторинга топологии
        _mMon.at(_idTopologyUUID)->startMonitoring();
        string strMsg = "The topology with UUID=" + _idTopologyUUID + " is started";
        _ptrLog->writeLineLevel1("Command: start_mon", strMsg);

    }
    catch(exception e)
    {
        string strMsg = "Error! The topology is UUID=" + _idTopologyUUID + " are not created!";
        _ptrLog->writeLineLevel1("Command: start_mon", strMsg);
    }
}


void CStopMonitoring::runCommand()
{
    _ptrLog->writeLineLevel1("Command: stop_mon",  "That is working command of the STOP monitoring");
    try
    {
        _mMon.at(_idTopologyUUID)->stopMonitoring();
        string strMsg = "The topology with UUID=" + _idTopologyUUID + " is stopped";
        _ptrLog->writeLineLevel1("Command: stop_mon", strMsg);
    }
    catch(std::exception e)
    {
        string strMsg = "Error! The topology is UUID=" + _idTopologyUUID + " are not stopped!";
        _ptrLog->writeLineLevel1("Command: stop_mon", strMsg);
    }

}


void CAddMonitoring::runCommand()
{
    string strMsg;
    _ptrLog->writeLineLevel1("Command: add", "That is working command of the adding new IP into monitoring\n");

    try
    {

        _mMon.at(_idTopologyUUID)->setListIp(_json);
        _mMon.at(_idTopologyUUID)->addMonitoring();
        strMsg = "The new list of ip-addresses is added into the topology with UUID=" + _idTopologyUUID;
        _ptrLog->writeLineLevel1("Command: add", strMsg);
    }
    catch(std::exception e)
    {
        strMsg = "Error of adding new list of ip-adresses! Topology is not exist!";
        _ptrLog->writeLineLevel1("Command: add", strMsg);
    }
}


void CDeleteMonitoring::runCommand()
{
    string strMsg;
    try
    {
         _mMon.at(_idTopologyUUID)->delListIp(_json);
         strMsg = "The new list of ip-addresses is deleted into the topology with UUID=" + _idTopologyUUID;
         _ptrLog->writeLineLevel1("command: del", strMsg);
    }
    catch(std::exception e)
    {
        strMsg = "Error of deliting list of ip-adresses! Topology is not exist!";
        _ptrLog->writeLineLevel1("command: del", strMsg);
    }
}

void CKillMonitoring::runCommand()
{
    string strMsg = "That is working command of the deleting thread monitoring from service by ID topology";
    _ptrLog->writeLineLevel1("command: kill", strMsg);
    // удаление топлогии из контейнера объектов мониторинга
    try
    {
        _mMon.at(_idTopologyUUID)->killMonitoring();
        _mMon.erase(_idTopologyUUID);
    }
    catch(std::exception e)
    {
        strMsg =  "Error of killing the topology. The topology with UUID " + _idTopologyUUID + "isn't exist!";
        _ptrLog->writeLineLevel1("command: kill", strMsg);
    }
}
