#include "curl.h"

Curl::Curl(CLog *ptrLog): _ptrHeaders(nullptr), _ptrLog(ptrLog)
{
    initCurl();
}

Curl::~Curl()
{
    freeCurl();
}

void Curl::postResponse()
{
    if(_ptrCurl)
    {
        _res = curl_easy_perform(_ptrCurl);
        if(CURLE_OK == _res)
        {
            curl_easy_getinfo(_ptrCurl, CURLINFO_RESPONSE_CODE, &_response_code);
            if(_ptrLog)
            {

                _ptrLog->writeLineLevel1("CURL",  _s.ptr);
            }
        }
        else
        {
            if(_ptrLog)
            {
                _ptrLog->writeLineLevel1("CURL error:", "the result response is not null");
            }
        }
    }
    else
    {
        if(_ptrLog)
            _ptrLog->writeLineLevel1("CURL", "running error");
    }
}

Curl::str Curl::getResultJSON()
{
    return _s;
}

bool Curl::is200ResponseCode()
{
    return 200 == _response_code;
}

void Curl::initCurl()
{
    curl_global_init(CURL_GLOBAL_ALL);
    _ptrCurl = curl_easy_init();

}

void Curl::freeCurl()
{

    /* always cleanup */
    curl_easy_cleanup(_ptrCurl);
    curl_global_cleanup();
}

void Curl::freeCurlSession()
{
       curl_easy_reset(_ptrCurl);
       free(_s.ptr);
      curl_slist_free_all(_ptrHeaders);
      _ptrHeaders = NULL;
}

void Curl::init_string(str *s)
{
    s->len = 0;
    s->ptr = (char*)malloc(s->len+1);
    if (s->ptr == NULL) {

        exit(EXIT_FAILURE);
    }
    s->ptr[0] = 0;
}

size_t Curl::writefunc(void *ptr, size_t size, size_t nmemb, str *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = (char*)realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {

        exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = 0;
    s->len = new_len;

    return size*nmemb;
}
