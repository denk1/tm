#ifndef CTHREAD_H
#define CTHREAD_H
#include <thread>
#include <list>
#include <stdlib.h>
#include <stdio.h>
#include <memory>



using namespace std;

//!  CThread - класс объектов потоков мониоринга
/*!
  реализует функционал управления потоками мониторинга
*/

class CThread
{
public:
    //! конструктор
    CThread();
    //! деструктор
    ~CThread();
protected:
    //! контейнер с потоками
    list<thread> _listThrd;
    //! идентификатор потока
    static int _indThread;
};

#endif
