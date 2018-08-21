#ifndef CURL_H
#define CURL_H
#include "clog.h"


    #include <curl/curl.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <cstring>

//!  Curl - класс объектов для отправки и получения запросов
/*!
  реализует функционал фунционал открытой библиотеки cURL(https://curl.haxx.se/)
*/


class Curl
{
public:
    //! структура строки запроса
    struct str {
     //! указатель на данные
      char *ptr;
     //! длинна данных
      size_t len;
    };
    //! конструктор
    Curl(CLog* ptrLog = NULL);
    //! деструктор
    ~Curl();
    //! отправка запроса
    void postResponse();
    //! получение результата
    str getResultJSON();
    //! проверка кода запроса
    bool is200ResponseCode();

protected:
    struct curl_slist *_ptrHeaders;
    CURL *_ptrCurl;
    CURLcode _res;
    int _sizeStr;
    struct str _s;

    const char* jsonObj = "empty";

    // ///////////////////////////
    // initialization CURL options
    // ///////////////////////////
    //! инициализация резурсов
    void initCurl();
    //! очистка ресурсов
    void freeCurl();
    void freeCurlSession();

    void init_string(str *s);
    //! метод для формирования строки результат
    static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct str *s);
    //! указатель на объект лога
    CLog* _ptrLog;
private:
    //! код http-запроса
    long _response_code;

};
#endif
