#include "cthread.h"


CThread::CThread()
{

}

CThread::~CThread()
{
    for(auto& th:_listThrd)
    {
//        th.detach();
//        th.join();
    }
}

int CThread::_indThread = 0;



