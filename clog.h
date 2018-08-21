#ifndef CLOG_H
#define CLOG_H

#include <string>
#include <fstream>
#include <ctime>
#include <iostream>
#include <exception>
#include <sstream>

using namespace std;

class CLog
{
public:
    CLog(string strPathFile);
    ~CLog();
    void setDebugLevel(size_t l);
    void writeLineLevel1(string nameMsg, string lineMsg);
    void writeLineLevel2(string nameMsg, string lineMsg);
    void writeLineLevel1(string nameMsg, int intValue);
    void writeLineLevel2(string nameMsg, int intValue);
private:
    ofstream f;
    string getCurrentTime();
    void messageError(exception *e);
    void writeLine(string nameMsg, string lineMsg);
    string getStrFromInt(int i);
    string _strPathFile;
    size_t _uDebugLevel;
};
#endif
