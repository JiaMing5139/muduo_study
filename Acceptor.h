//
// Created by parallels on 5/13/20.
//

#ifndef MUDUO_STUDY_ACCEPTOR_H
#define MUDUO_STUDY_ACCEPTOR_H

#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
class EventLoop;
class TcpConnection;

class Acceptor {
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionptr;
    typedef std::function<void (TcpConnectionptr)> NewConnectionCallback;

    explicit Acceptor(const InetAddress &,EventLoop * loop,NewConnectionCallback);
     Acceptor(const InetAddress &,EventLoop * loop);
    void setNewConnectionCallback(NewConnectionCallback cb){newConnectionCallback=cb;}
    void listen();

private:
    NewConnectionCallback newConnectionCallback;
    void handleReadEvent();

    EventLoop* loop_;
    Socket listenfd_;
    Channel::Channelptr channel_;


};


#endif //MUDUO_STUDY_ACCEPTOR_H
