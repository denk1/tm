#include "clog.h"

CLog::CLog(string strPathFile):_uDebugLevel(1)
{
    try
    {
        f.exceptions ( std::ofstream::failbit | std::ofstream::badbit );
        f.open(strPathFile, ios_base::out | ios_base::app);
    }
    catch(std::ofstream::failure e)
    {
        messageError(&e);
    }
}

CLog::~CLog()
{
    if(f.is_open())
    {
        f.close();
    }
}

void CLog::setDebugLevel(size_t levelNum)
{
    _uDebugLevel = levelNum;
}

void CLog::writeLineLevel1(string nameMsg, string lineMsg)
{
    writeLine(nameMsg, lineMsg);
}

void CLog::writeLineLevel2(string nameMsg, string lineMsg)
{
    if(_uDebugLevel>1)
        writeLine(nameMsg, lineMsg);
}

void CLog::writeLineLevel1(string nameMsg, int intValue)
{
    writeLine(nameMsg, getStrFromInt(intValue));
}

void CLog::writeLineLevel2(string nameMsg, int intValue)
{
    if(_uDebugLevel>1)
        writeLine(nameMsg, getStrFromInt(intValue));
}

void CLog::writeLine(string nameMsg, string lineMsg)
{
    try
    {
        string written_line = "[" + getCurrentTime() + "] " + nameMsg + ": " + lineMsg;
        f << written_line << endl;
    }
    catch(std::ofstream::failure e)
    {
        messageError(&e);
    }
}

string CLog::getStrFromInt(int i)
{
    ostringstream o;
    o << i;
    return o.str();
}

string CLog::getCurrentTime()
{
    std::time_t result = std::time(nullptr);
    struct tm * timeinfo = std::localtime(&result);
    char buffer[80];
    strftime (buffer,80,"%d.%m.%y %X",timeinfo);
    return string(buffer);
}

void CLog::messageError(exception* e)
{
    std::cerr << "Exception opening/reading/closing log-file\n" << endl;
    std::cerr << "Must be log-file into path /var/log/stepler/tm\n" << endl;
    std::cerr << e->what();
    exit(1);
}
