//
// Created by parallels on 3/31/20.
//

#ifndef TCPSERVER_REQUESTHEADER_H
#define TCPSERVER_REQUESTHEADER_H

#include <string>
using std::string;
class RequestHeader {
private:
    string Host;
    string Connection;
    string Content_length;
    string Cache;
    string Content_Type;
    string User_Agent;
    string Accept;
    string Referer;
    string Language;
    string Cookie;
};


#endif //TCPSERVER_REQUESTHEADER_H
