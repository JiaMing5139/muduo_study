//
// Created by parallels on 3/31/20.
//

#ifndef TCPSERVER_HTTPREQUEST_H
#define TCPSERVER_HTTPREQUEST_H

#include <string>
#include <unordered_map>
#include <map>
#include <iostream>
#include "http.h"
class httpRequest {
    friend std::ostream &operator << (std::ostream & os , const httpRequest &);
public:
    httpRequest() = default;

    enum  statusProcessing  {
        RequestLine,
        Header,
        Data,
        Finish
    };
    bool parseContext(const std::string &context);
    std::string getUrl(){return this->_url;}
    Method  getMethod(){return this->_method;}
    bool ifFinish(){return _status == Finish;}
private:
    void setUrl(const std::string & s );
    bool setMethod(const std::string & );
    void setVersion(const std::string & s){ this->_version = s;}; //fixme no


    bool parseRequestLine(const std::string & line);
    bool parseHeader(const std::string & line);

    Method _method = INVAILD;
    std::string _url;
    std::string _query;
    std::string _version;
    statusProcessing _status = RequestLine;
    std::unordered_map<std::string,std::string> _data;
    std::map<std::string,std::string> _header;



};


#endif //TCPSERVER_HTTPREQUEST_H
