#include "cjson.h"

CJSON::CJSON(string str):_strJSON(str), _ptrJSON(json_tokener_parse(str.c_str()))
{

}

CJSON::CJSON():_strJSON(), _ptrJSON(json_object_new_object())
{

}

CJSON::~CJSON()
{

    json_object_put(_ptrJSON);
}

CJSON::CJSON(const CJSON& cjson)
{


    _strJSON = cjson._strJSON;
    _ptrJSON = json_object_get(cjson._ptrJSON);
}

CJSON::CJSON(const CJSON &&cjson):_strJSON(cjson._strJSON)
{
    _ptrJSON = json_object_get(cjson._ptrJSON);
}

 void CJSON::operator=(const CJSON &jObj)
 {
    _strJSON = jObj._strJSON;
    json_object_put(_ptrJSON);
    _ptrJSON = json_object_get(jObj._ptrJSON);
 }

void CJSON::operator=(const CJSON&& jObj)
{
    _strJSON = jObj._strJSON;
    json_object_put(_ptrJSON);
    _ptrJSON = json_object_get(jObj._ptrJSON);
}

 string CJSON::dump() const
 {

    return json_object_to_json_string(_ptrJSON);
 }

 json_object* CJSON::getPtrJsonObj()
 {
     return _ptrJSON;
 }

 void CJSON::setPtrJsonObj(json_object *jObj)
 {
     json_object_put(_ptrJSON);
     _ptrJSON = json_object_get(jObj);
     _strJSON = json_object_to_json_string(jObj);
 }
