#include "cxml.h"





CXml::CXml(CLog *ptrLog): num(0),
    _doc(NULL),
    _ptrLog(ptrLog)
{

}

CXml::~CXml()
{

}

void CXml::readNode(xmlNodePtr node)
{
    xmlNodePtr cur_node = node;
    while (cur_node)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if(xmlStrEqual(cur_node->name, (xmlChar*)"host"))
            {

                xmlNodePtr curChildrenNodeTrace = cur_node->children;
                while(curChildrenNodeTrace)
                {
                    if(xmlStrEqual(curChildrenNodeTrace->name, (xmlChar*)"trace"))
                    {
                        std::string strLast = _strLocalHostIp;
                        xmlNodePtr curChildrenNodeHop = curChildrenNodeTrace->children;
                        while(curChildrenNodeHop)
                        {
                            if(xmlStrEqual(curChildrenNodeHop->name, (xmlChar*)"hop"))
                            {
                                xmlAttrPtr attrHope = curChildrenNodeHop->properties;

                                while(attrHope)
                                {
                                    if(xmlStrEqual(attrHope->name, (xmlChar*)"ipaddr"))
                                    {

                                        std::string strIp((const char*)attrHope->children->content);
                                        LinkHosts linkHosts(strLast, strIp);
                                        VectorLinkHosts::iterator itLinkHosts = std::find(vLinkHosts.begin(), vLinkHosts.end(), linkHosts);
                                        // отладочная запись
                                        _ptrLog->writeLineLevel2(strLast, strIp);
                                        if(itLinkHosts==vLinkHosts.end() && !linkHosts.strIp1.empty() && !linkHosts.strIp2.empty())
                                        {

                                            vLinkHosts.push_back(linkHosts);
                                        }
                                        strLast = strIp;
                                    }
                                    attrHope = attrHope->next;
                                }
                            }
                            curChildrenNodeHop = curChildrenNodeHop->next;
                        }
                    }
                    curChildrenNodeTrace = curChildrenNodeTrace->next;
                }
            }
        }
        readNode(cur_node->children);
        cur_node = cur_node->next;
    }
}

void CXml::setLocalIp(xmlNodePtr node)
{
    xmlNodePtr cur_node = node;
    _flagStatusTag = false;
    while (cur_node)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            if(xmlStrEqual(cur_node->name, (xmlChar*)"host"))
            {

                xmlNodePtr curChildrenNodeTrace = cur_node->children;
                while(curChildrenNodeTrace)
                {
                    if(xmlStrEqual(curChildrenNodeTrace->name, (xmlChar*)"address"))
                    {
                        xmlAttrPtr attr = curChildrenNodeTrace->properties;
                        while(attr)
                        {
                            if(xmlStrEqual(attr->name, (xmlChar*)"addr"))
                            {

                                if(_flagStatusTag)
                                {
                                   _strLocalHostIp = std::string((const char*)attr->children->content);
                                }
                            }
                            attr = attr->next;
                        }
                    }

                    if(xmlStrEqual(curChildrenNodeTrace->name, (xmlChar*)"status"))
                    {
                        xmlAttrPtr attr = curChildrenNodeTrace->properties;
                        while(attr)
                        {
                            if(xmlStrEqual(attr->name, (xmlChar*)"reason") && xmlStrEqual(attr->children->content, (xmlChar*)"localhost-response"))
                            {
                                _flagStatusTag = true;
                            }
                            attr = attr->next;
                        }
                    }

                    curChildrenNodeTrace = curChildrenNodeTrace->next;
                }
            }
        }
        setLocalIp(cur_node->children);
        cur_node = cur_node->next;
    }
}

void CXml::ReadXML(std::string &strResult)
{

    _ptrLog->writeLineLevel2("list", "link");
//    _doc = xmlReadFile("result.xml", NULL, 0);
    _doc = xmlReadMemory(strResult.c_str(), strResult.size(), "noname.xml", NULL, 0);
    if(_doc == NULL)
    {
        std::cout << "Error parsing stream data from streaming output nmap " << std::endl;
        return;
    }
    _cur = xmlDocGetRootElement(_doc);
    _curLocalIp = xmlDocGetRootElement(_doc);
    setLocalIp(_curLocalIp);
    readNode(_cur);
    if(_doc != NULL)
    {
        xmlFreeDoc(_doc);
    }
}

VectorLinkHosts &CXml::getLinkHost()
{
    return vLinkHosts;
}

LinkHosts::LinkHosts(std::string ip1, std::string ip2):strIp1(ip1), strIp2(ip2)
{

}

bool LinkHosts::operator==(const LinkHosts &linkHost) const
{
    return (strIp1 == linkHost.strIp1 && strIp2 == linkHost.strIp2) || (strIp1 == linkHost.strIp2 && strIp2 == linkHost.strIp1);
}

bool LinkHosts::operator<(const LinkHosts &linkHosts) const
{
    if(strIp1 < linkHosts.strIp1)
        return true;
    if(strIp1 == linkHosts.strIp1 && strIp2 < linkHosts.strIp2)
        return true;
    return false;
}
