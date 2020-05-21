//
// Created by parallels on 3/31/20.
//

#include "httpRequest.h"
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <iostream>
using std::string;
using std::cout;
using std::endl;

bool httpRequest::parseContext(const std::string &context) {
    std::istringstream is(context);
    std::string line;

    while(std::getline(is,line)) {
       // cout << "current lin:" << line << endl;
        if(_status == RequestLine){
            if(line == "\r" || line.empty()){
                break;
            }
            if(!parseRequestLine(line)) return  false;
            _status = Header;
        }else if(_status == Header){
            if(line == "\r" || line.empty()){
                _status = Data;
                _status = Finish;
                break;
            }
            if(!parseHeader(line)){
                return false;
            }
        }else if(_status == Data){
            std::cout << "start deal with data" << endl;
        }

        }





}


bool httpRequest::parseRequestLine(const std::string &req) {
    std::istringstream is(req);
    bool ifSuccess = false;
    enum status{metod,path,version,done};
    status currentStatus = metod;
    std:: string tmp;
    while (is >> tmp){
        switch (currentStatus){
            case metod:
                if(!setMethod(tmp)) return  false;
                currentStatus = path;
                break;
            case path:
                setUrl(tmp);
                currentStatus = version;
                break;
            case version:
                setVersion(tmp);
                currentStatus = done;
                break;
            case done:
                if(!tmp.empty()) return false;
        }

    }

    return true;


}

bool httpRequest::parseHeader(const std::string & line) {
    std::string _line = line;
    std::string::iterator colon = std::find(_line.begin(),_line.end(),':');
    std::string field(_line.begin(), colon);
    if (colon ==  _line.end()){
        return  false;
    }

    ++colon;
    while (colon < _line.end() && isspace(*colon))
    {
        ++colon;
    }
    std::string value(colon, _line.end());
    while (!value.empty() && isspace(value[value.size()-1]))
    {
        value.resize(value.size()-1);
    }
    _header[field] = value;
    return  true;

}

bool httpRequest::setMethod(const std::string & met) {
  if (met == "GET") {
      this->_method = GET;
  }else if(met == "POST"){
      this->_method = POST;
  }else if(met == "DELETE"){
      this->_method = DELETE;
  }else if(met == "PUT"){
      this->_method = PUT;
  }else{
      this->_method = INVAILD;
  }
    return _method!= INVAILD;
}

void httpRequest::setUrl(const std::string & line) {
    std::string _line = line;
    std::string::iterator quesiton = std::find(_line.begin(),_line.end(),'?');
    std::string url(_line.begin(), quesiton);
    if (quesiton != _line.end()) {
        ++quesiton;
        while (quesiton < _line.end() && isspace(*quesiton)) {
            ++quesiton;
        }
        std::string query(quesiton, _line.end());
        while (!query.empty() && isspace(query[query.size() - 1])) {
            query.resize(query.size() - 1);
        }
        this->_query = query;
       // cout << _query << endl;
    }
    this->_url = url;
   // cout << url << endl;


}

std::ostream &operator<<(std::ostream &os, const httpRequest & hr) {
    os << "method:"<< hr._method << endl;
    os << "url:"<< hr._url << endl;
    os << "query:"<<hr._query << endl;
    os << "version:"<<hr._version << endl;
    os << "header:" << endl;
    for(auto p : hr._header){
        os << p.first<<":" <<p.second << endl;
    }
    return os;
}


