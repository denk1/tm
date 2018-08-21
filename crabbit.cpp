#include "crabbit.h"


CRabbit::CRabbit(const char *chIpAdd, const char *chQueueName, const char *chUserNane, const char *chPswd, CLog *ptrLog, string strAdrSrv, unsigned int pause):
    _commandFactory(ptrLog, pause)
{
    _chIpAddr = chIpAdd;
    _chQueueName = chQueueName;
    _chUserName = chUserNane;
    _chPswd = chPswd;
    _ptrLog = ptrLog;
    _commandFactory.setStrAdrr(strAdrSrv);
}

CRabbit::~CRabbit()
{
    _ptrThread->detach();


}

void CRabbit::startRabbit()
{
    //_ptrThread = std::shared_ptr<std::thread>(new std::thread(&CRabbit::run, this));
    run();
}

void CRabbit::runConsumer(amqp_connection_state_t conn)
{
    uint64_t start_time = now_microseconds();
    int received = 0;
    int previous_received = 0;
    uint64_t previous_report_time = start_time;
    uint64_t next_summary_time = start_time + SUMMARY_EVERY_US;

    amqp_frame_t frame;

    uint64_t now;

    for (;;) {
      amqp_rpc_reply_t ret;
      amqp_envelope_t envelope;
      now = now_microseconds();
      if (now > next_summary_time) {
        int countOverInterval = received - previous_received;
        double intervalRate = countOverInterval / ((now - previous_report_time) / 1000000.0);
//        sprintf("%d ms: Received %d - %d since last report (%d Hz)\n",
//               (int)(now - start_time) / 1000, received, countOverInterval, (int) intervalRate);
        _ptrLog->writeLineLevel1("RabbitMQ:","info:");
        _ptrLog->writeLineLevel1("ms",  (int)(now - start_time) / 1000);
        _ptrLog->writeLineLevel1("received", received);
        _ptrLog->writeLineLevel1("since last report", countOverInterval);
        _ptrLog->writeLineLevel1("Hz", (int) intervalRate);
        previous_received = received;
        previous_report_time = now;
        next_summary_time += SUMMARY_EVERY_US;
      }
      amqp_maybe_release_buffers(conn);
      ret = amqp_consume_message(conn, &envelope, NULL, 0);
      size_t len = envelope.message.body.len;
      const char* ch = (const char*)envelope.message.body.bytes;
      string strJson(ch, len);
      _ptrLog->writeLineLevel1("test", strJson);
      // проверка поступления пустого сообщения кролика
      if(!strJson.empty())
      {
          CJSON jObj(strJson);
          shared_ptr<Command> ptrCommand = _commandFactory.createCommand(jObj);
          ptrCommand->runCommand();
          _ptrLog->writeLineLevel1("thread count:", ptrCommand->getCountThread());
      }
      else
      {
          _ptrLog->writeLineLevel1("empty message from rabbitmq server", "!!!");
      }
      if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
        if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
            AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
          if (AMQP_STATUS_OK != amqp_simple_wait_frame(conn, &frame)) {
            return;
          }

          if (AMQP_FRAME_METHOD == frame.frame_type) {
            switch (frame.payload.method.id) {
              case AMQP_BASIC_ACK_METHOD:
                /* if we've turned publisher confirms on, and we've published a message
                 * here is a message being confirmed
                 */
                printf("AMQP_BASIC_ACK_METHOD\n");
                break;
              case AMQP_BASIC_RETURN_METHOD:
                /* if a published message couldn't be routed and the mandatory flag was set
                 * this is what would be returned. The message then needs to be read.
                 */
                {
                  amqp_message_t message;
                  ret = amqp_read_message(conn, frame.channel, &message, 0);
                  if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
                    return;
                  }

                  amqp_destroy_message(&message);
                }
                printf("AMQP_BASIC_RETURN_METHOD\n");
                break;

              case AMQP_CHANNEL_CLOSE_METHOD:
                /* a channel.close method happens when a channel exception occurs, this
                 * can happen by publishing to an exchange that doesn't exist for example
                 *
                 * In this case you would need to open another channel redeclare any queues
                 * that were declared auto-delete, and restart any consumers that were attached
                 * to the previous channel
                 */
                return;

              case AMQP_CONNECTION_CLOSE_METHOD:
                /* a connection.close method happens when a connection exception occurs,
                 * this can happen by trying to use a channel that isn't open for example.
                 *
                 * In this case the whole connection must be restarted.
                 */
                return;

              default:
                fprintf(stderr ,"An unexpected method was received %u\n", frame.payload.method.id);
                return;
            }
          }
        }

      } else {
           amqp_destroy_envelope(&envelope);
      }

      received++;
    }
}

void CRabbit::run()
{
    const char *queue_name = _chQueueName;
    char const *hostname;
    int port, status;
    char const *exchange;
    char const *bindingkey;
    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;

    amqp_bytes_t queuename;
    hostname = _chIpAddr;
//    hostname = "93.158.192.230";
    port = 5672;
    exchange = "amq.direct"; /* argv[3]; */
    bindingkey = ""; /* argv[4]; */

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
      die("creating TCP socket");
    }

    status = amqp_socket_open(socket, hostname, port);
    if (status) {
      die("opening TCP socket");
    }

    die_on_amqp_error(amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, _chUserName, _chPswd),
                      "Logging in");
    amqp_channel_open(conn, 1);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Opening channel");

    {
        amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1,    amqp_cstring_bytes(queue_name), 0, 1, 0, 0,
                                         amqp_empty_table);

  /*    amqp_queue_declare_ok_t *r = amqp_queue_declare(conn, 1, amqp_empty_bytes, 0, 0, 0, 1,
                                   amqp_empty_table); */
      die_on_amqp_error(amqp_get_rpc_reply(conn), "Declaring queue");
      queuename = amqp_bytes_malloc_dup(r->queue);
    }

    amqp_queue_bind(conn, 1, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey),
                    amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Binding queue");

    amqp_basic_consume(conn, 1, queuename, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    die_on_amqp_error(amqp_get_rpc_reply(conn), "Consuming");

    runConsumer(conn);

    die_on_amqp_error(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing channel");
    die_on_amqp_error(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing connection");
    die_on_error(amqp_destroy_connection(conn), "Ending connection");
}
