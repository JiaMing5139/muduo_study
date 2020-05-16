//
// Created by parallels on 5/13/20.
//

#ifndef MUDUO_STUDY_TCPSERVER_H
#define MUDUO_STUDY_TCPSERVER_H

#include "Acceptor.h"
#include "InetAddress.h"
#include <map>
#include <vector>
#include "base/CallBack.h"
class EventLoop;
class TcpServer {
public:

    typedef std::shared_ptr<TcpConnection> TcpConnectionptr;
    typedef std::map<int,TcpConnectionptr> TcpConnectionMap;

    TcpServer(const InetAddress &addr,EventLoop *loop);

    void newConnectionCallback(int fd, const InetAddress & inetAddress);


    void setOnMessageCallback(readTcpEventCallback cb){onMessage_ =cb;}
    void setOnConnectionCallback(TcpEventCallback cb){onConnection_ =cb;}
    void setOnWriteCompleteCallback(TcpEventCallback cb){writeCompleteCallback =cb;}

    void removeTcpConnection(const TcpConnectionptr &);
private:

    readTcpEventCallback onMessage_;

    TcpEventCallback onConnection_;
    TcpEventCallback closeCallback;
    TcpEventCallback writeCompleteCallback;

    std::vector<TcpConnectionptr> tcpConnectionList_;
    TcpConnectionMap tcpConnectionMap_;
    Acceptor acceptor_;
    EventLoop* loop_;
    InetAddress localaddr_;


};


#endif //MUDUO_STUDY_TCPSERVER_H
