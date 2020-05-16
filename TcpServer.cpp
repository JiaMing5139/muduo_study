//
// Created by parallels on 5/13/20.
//

#include "TcpServer.h"
#include "log/logger.h"
#include "TcpConnection.h"
#include <assert.h>
#include "EventLoop.h"
TcpServer::TcpServer(const InetAddress &addr,EventLoop *loop):
acceptor_(addr,loop),
loop_(loop),
localaddr_(addr)
{
    acceptor_.setNewConnectionCallback(bind(&TcpServer::newConnectionCallback,this,std::placeholders::_1,std::placeholders::_2));
    acceptor_.listen();
}

void TcpServer::newConnectionCallback( int fd, const InetAddress & peerAddress) {
    TcpConnectionptr tcpConnectionptr(new TcpConnection(loop_,fd),[](TcpConnection * conn){
        LOG_TRACE<< "Deleter of Tcpconnection";
        delete conn;
    });
    tcpConnectionptr->setInetAddress(peerAddress,localaddr_);

    LOG_TRACE<<"Tcpserver: NewConnectionacallback:" <<tcpConnectionptr->peerAddr()<<"->"
             << tcpConnectionptr->localAddr() <<" refcount " << tcpConnectionptr.use_count();
    tcpConnectionptr->setOnMessageCallback(onMessage_);
    tcpConnectionptr->setOnConnectionCallback(onConnection_);
    tcpConnectionptr->setOnClosedCallback(bind(&TcpServer::removeTcpConnection,this,std::placeholders::_1));
    tcpConnectionptr->enableRead();
    tcpConnectionMap_[fd] = std::move(tcpConnectionptr);
    LOG_TRACE<<"refcout of connection" <<tcpConnectionMap_[fd].use_count();

}

void TcpServer::removeTcpConnection(const TcpServer::TcpConnectionptr &conn) {
    loop_->assertInLoopThread();
    LOG_TRACE <<"try to remove TcpConnection from TcpServer current :refcount of conn is " << conn.use_count();
    int ret = tcpConnectionMap_.erase(conn->fd());
    assert(ret != 0);
    //TcpConnection dtor here
}

