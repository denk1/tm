#include "crun.h"
#include <cstdlib>
#include "conf.h"
#include "crabbit.h"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include "clog.h"

using namespace std;
//! главная точка входа в программу мониторинга
int main(int argc, char** argv)
{
    uid_t uid=getuid(), euid=geteuid();
    if (uid==0/* || uid!=euid*/) {
        /* We might have elevated privileges beyond that of the user who invoked
         * the program, due to suid bit. Be very careful about trusting any data! */
        //! инициализация:
        //! объект для работы с конфигурационным файлом сервиса
        Conf* ptrConf = NULL;
        if(argc > 1)
        {
            if(!strcmp(argv[1], "-c"))
                ptrConf = new Conf(argv[2]);
            else
            {
                cout << "unknown key " << argv[1] << endl;
                cout << "example of using:" << endl;
                cout << "stepler-topology-monitor [-c /path/to/config_file.conf] " << endl;
            }
        }
        else
        {
            ptrConf = new Conf();
        }
        ptrConf->getConfig();
       CLog log(string(ptrConf->getValueByName("LOG_FILE_PATH")) + "topology-monitor.log");
       log.setDebugLevel(1);
        //! объект главного цикла микросервиса
        CRun run(ptrConf->getValueByName("ADDR_FSS_BACKEND_SERVER"), &log, atoi(ptrConf->getValueByName("TIME_WAIT_SCAN")));
        //! получение текущих рабочих параметров работы сервиса мониторинга
        //! запуск соединения с RabbitMQ-сервером
        //! set timeout of the scan hosts
        //! установка частоты сканирования по умолчанию
        run.setTimeout(atoi(ptrConf->getValueByName("TIME_WAIT_SCAN")));
        //! получение аутентифицирующих данных
        run.setAuthData(string(ptrConf->getValueByName("USER_NAME")), string(ptrConf->getValueByName("PASSWORD")));
        //! запуск мониторинга сервиса
        run.start();
        //! объет для работы с RabbitMQ-cервером
        CRabbit rabbit(ptrConf->getValueByName("ADDR_RABBITMQ_SERVER"),
                       ptrConf->getValueByName("QUEUE_NAME_RABBITMQ_SERVER"),
                       ptrConf->getValueByName("USER_NAME_RABBITMQ_SERVER"),
                       ptrConf->getValueByName("PASSWORD_RABBITMQ_SERVER"),
                       &log,
                       ptrConf->getValueByName("ADDR_FSS_BACKEND_SERVER"),
                       atoi(ptrConf->getValueByName("TIME_WAIT_SCAN"))
                       );
        rabbit.startRabbit();
        //while (1);
        delete ptrConf;
    }
    else
    {
        cout << "That is program due run as root only" << endl;
        exit(1);
    }
}

