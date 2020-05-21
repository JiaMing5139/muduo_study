//
// Created by parallels on 3/31/20.
//

#ifndef TCPSERVER_RESPONSE_H
#define TCPSERVER_RESPONSE_H

#include <map>
#include <string>
using std::string;


class  Buffer;
class Response {
public:
    enum HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };
    void setStatusCode(HttpStatusCode code)
    { statusCode_ = code; }

    void setStatusMessage(const string& message)
    { statusMessage_ = message; }

    void setCloseConnection(bool on)
    { closeConnection_ = on; }

    bool closeConnection() const
    { return closeConnection_; }

    void setContentType(const string& contentType)
    { addHeader("Content-Type", contentType); }


    void addHeader(const string& key, const string& value)
    { headers_[key] = value; }

    void setBody(const string& body)
    { body_ = body; }

    void appendToBuff(Buffer* output) const;

private:
    std::map<string, string> headers_;
    HttpStatusCode statusCode_;
    string statusMessage_;
    bool closeConnection_;
    string body_;
};


#endif //TCPSERVER_RESPONSE_H
