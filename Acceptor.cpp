//
// Created by parallels on 5/13/20.
//

#include "Acceptor.h"

#include <utility>
#include "SocketsOps.h"
#include "Channel.h"
#include "TcpConnection.h"
#include "log/logger.h"
Acceptor::Acceptor(const InetAddress &,EventLoop * loop,NewConnectionCallback cb):
loop_(loop),
listenfd_(sockets::createNonblockingOrDie(AF_INET)),
channel_(new Channel(loop_,listenfd_.fd())),
newConnectionCallback(std::move(cb))
{

}

Acceptor::Acceptor(const InetAddress &addr,EventLoop * loop):
        loop_(loop),
        listenfd_(sockets::createNonblockingOrDie(AF_INET)),
        channel_(new Channel(loop_,listenfd_.fd()))
{
    LOG_TRACE <<"Accepotr created listenfd:" << channel_->fd() ;
    listenfd_.bindAddress(addr);
    listenfd_.setReuseAddr(true);
    listenfd_.setReusePort(true);
}



void Acceptor::handleReadEvent() {
    InetAddress peerAddr;
    int fd = listenfd_.accept(&peerAddr);
    TcpConnectionptr connectionptr(new TcpConnection);
    if(newConnectionCallback){
        newConnectionCallback(std::move(connectionptr));
    }

}

void Acceptor::listen() {
    printf("acceptor listen\n");
    listenfd_.listen();
    channel_->setReadCallBack(std::bind(&Acceptor::handleReadEvent,this));
    channel_->enableRead();
}
