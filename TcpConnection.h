//
// Created by parallels on 5/13/20.
//

#ifndef MUDUO_STUDY_TCPCONNECTION_H
#define MUDUO_STUDY_TCPCONNECTION_H

#include "base/CallBack.h"
#include <memory>
#include "InetAddress.h"
#include "base/noncopyable.h"
class Channel;
class EventLoop;
class TcpConnection: public std::enable_shared_from_this<TcpConnection>, public noncopyable  {
public:
     enum status{
         kConnected,
         closed
     };

    typedef std::shared_ptr<Channel> Channelptr;
    typedef std::shared_ptr<TcpConnection> TcpConnectionptr;
    explicit TcpConnection(EventLoop *, int fd);
    int fd() const {return sockfd_;}
    void enableRead();


    void setInetAddress(const InetAddress &peerAddr,const InetAddress &localAddr){
        peerAddr_ = peerAddr;
        localAddr_ = localAddr;
    }
    InetAddress localAddr() const { return localAddr_;}
    InetAddress peerAddr() const { return peerAddr_;}
    void cancel();
    void handReadEvent();
    void handleCloseEvent();


    void setOnMessageCallback(readTcpEventCallback cb){onMessage_ =cb;}
    void setOnConnectionCallback(TcpEventCallback cb){onConnection_ =cb;}
    void setOnClosedCallback(TcpEventCallback cb){closeCallback = cb;}
    void setOnWriteCompleteCallback(TcpEventCallback cb){writeCompleteCallback =cb;}


private:
    readTcpEventCallback onMessage_;

    TcpEventCallback onConnection_;
    TcpEventCallback closeCallback;
    TcpEventCallback writeCompleteCallback;

    status kStatus_ = closed;

    InetAddress peerAddr_;
    InetAddress localAddr_;

    Channelptr channel_;
    const int sockfd_;
};


#endif //MUDUO_STUDY_TCPCONNECTION_H