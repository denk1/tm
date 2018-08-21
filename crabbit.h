#ifndef CRABBIT_H
#define CRABBIT_H
#include <thread>
#include <vector>
#include <memory>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>

#include <assert.h>
#include "utils.h"
#include "clog.h"
#include "cjson.h"
#include "command.h"

#define SUMMARY_EVERY_US 1000000

extern void die(const char *fmt, ...);
extern  void die_on_error(int x, char const *context);
extern  void die_on_amqp_error(amqp_rpc_reply_t x, char const *context);

extern  void amqp_dump(void const *buffer, size_t len);

extern  uint64_t now_microseconds(void);
extern  void microsleep(int usec);


using namespace std;

//!  CRabbit -ласс для работы  с RabbitMQ-сервером (https://www.rabbitmq.com/)
/*!
  инкапсулирует функционал отправки и получения сообщений в очередях
*/


class CRabbit
{
public:
    //! конструктор
    CRabbit(
            const char* chIpAdd,
            const char* chQueueName,
            const char* chUserNane,
            const char* chPswd,
            CLog *ptrLog,
            string strAdrSrv,
            unsigned int pause
            );
    //! деструктор
    ~CRabbit();
    //! старт потока, "слушающего" определенную очередь на RabbitMQ-сервере
    void startRabbit();
    void run();
private:
    //! указатель на объект "слушающего" потока
    std::shared_ptr<std::thread> _ptrThread;
    //! цикл поддержания соединения с RabbitMQ-сервером
    void runConsumer(amqp_connection_state_t conn);
    //! строка адреса rabbitmq-сервера
    const char *_chIpAddr, *_chQueueName, *_chUserName, *_chPswd;
    //! указатель на объект лога
    CLog *_ptrLog;
    //! абстрактная фабрика объектов команд
    CommandFactory _commandFactory;

};

#endif
