#ifndef CXML_H
#define CXML_H
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlwriter.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <iostream>
#include "clog.h"

#define MY_ENCODING "UTF-8"

using namespace  std;

struct LinkHosts;
typedef std::vector<LinkHosts> VectorLinkHosts;

struct LinkHosts
{
    std::string strIp1, strIp2;
    LinkHosts(std::string ip1, std::string ip2);
    bool operator==(const LinkHosts& linkHost) const;
    bool operator<(const LinkHosts& LinkHosts) const;
};


//!  CXml - класс для обрабоки данных в формате xml
/*!
  реализует функционал обрабоки хml-данных
*/

class CXml
{
public:
    //! коструктор
    CXml(CLog* ptrLog);
    //! деструктор
    ~CXml();
    //! чтение
    void ReadXML(std::string& strResult);
    //! контейнер со списком связей
    VectorLinkHosts& getLinkHost();
private:
    //! чтение xml
    void readNode(xmlNodePtr node);
    //! установка локального ip-адреса
    void setLocalIp(xmlNodePtr node);
    //! объекты xml-докумета
    xmlDocPtr _doc;
    xmlNodePtr _cur, _curLocalIp;
    //! порядковый номер хоста
    unsigned int num;
    //! контейнер со списком ссылок
    VectorLinkHosts vLinkHosts;
    //! название корня xml-документа
    const char *uri = "topology";
    //! строка с ip-адресом локального хоста
    std::string _strLocalHostIp;
    //! статус уровня вложенности xml-документа
    bool _flagStatusTag;
    //! указатель на объект лога
    CLog* _ptrLog;
};

#endif
