#ifndef CJSON_H
#define CJSON_H
#include <string>
#include <exception>
//#include <json/json.h>
#include <json-c/json.h>
#include "clog.h"
#include <cstring>
#include <list>


using namespace std;

class CJSON {
public:
    CJSON();
    ~CJSON();
    CJSON(string str);
    CJSON(const CJSON& cjson);
    CJSON(const CJSON&& cjson);
    const char* getValueString(string strKey)
    {
        json_object* jObj;
        json_object_object_get_ex(_ptrJSON, strKey.c_str(), &jObj);
        return json_object_get_string(jObj);
    }

    bool getValueBool(string strKey)
    {
        json_object* jObj;
        json_object_object_get_ex(_ptrJSON, strKey.c_str(), &jObj);
        return json_object_get_boolean(jObj);
    }

    int getValueInt(string strKey)
    {
        json_object* jObj;
        json_object_object_get_ex(_ptrJSON, strKey.c_str(), &jObj);
        return json_object_get_int(jObj);
    }

    double getValueDouble(string strKey)
    {
        json_object* jObj;
        json_object_object_get_ex(_ptrJSON, strKey.c_str(), &jObj);
        return json_object_get_double(jObj);
    }

    CJSON getValueJsonObj(string strKey)
    {
        json_object* jObj;
        json_object_object_get_ex(_ptrJSON, strKey.c_str(), &jObj);
        CJSON cjson;
        cjson.setPtrJsonObj(jObj);
        return cjson;
    }

    json_object* getValueJsonArray(string strKey)
    {
        json_object* jObj;
        json_object_object_get_ex(_ptrJSON, strKey.c_str(), &jObj);
        return jObj;
    }

    void operator=(const CJSON& jObj);
    void operator=(const CJSON&& jObj);
    string dump() const;
    json_object* getPtrJsonObj();
    void setPtrJsonObj(json_object *jObj);

protected:
    string _strJSON;
    json_object* _ptrJSON;
private:
    void freeJsonObj(json_object* ptrJsonObj);
    void getJsonObj(json_object* ptrJsonObj);
};

#endif
