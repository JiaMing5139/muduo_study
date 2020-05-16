//
// Created by parallels on 5/13/20.
//

#include "TcpConnection.h"
#include "Channel.h"
#include "log/logger.h"

#include <assert.h>

TcpConnection::TcpConnection(EventLoop *loop,int fd):
sockfd_(fd),
channel_(new Channel(loop,fd),[](Channel * chanptr){
    delete chanptr;
    LOG_TRACE<< "Deleter of Channel";
}){  // refcount of channel =  1
//    channel_->enableWrite();
    LOG_TRACE << "refcount of channel in ctor of TcpConnection:" << this->channel_.use_count();
}

void TcpConnection::enableRead() {
    assert(kStatus_ == closed);

    channel_->setReadCallBack(bind(&TcpConnection::handReadEvent,this));
    LOG_TRACE << "refcount of channel in enable read 1:" << this->channel_.use_count();
    channel_->enableRead();//refcount of channel =  2 channelMap +1
    LOG_TRACE << "refcount of channel in enable read 2:" << this->channel_.use_count();
    assert(onConnection_);
    onConnection_(shared_from_this());
    kStatus_ = kConnected;
}

void TcpConnection::cancel() {
    if(kStatus_ == kConnected) {
        channel_->removeself();
    }
}

void TcpConnection::handReadEvent() {
    //refcount of channel =  4  | one is in channelMap of Poll, and another is in activedChannels;
    assert(onMessage_);
    char buff[512];
    int n = recv(this->fd(),&buff,sizeof buff,0);
    if(n == 0){
        handleCloseEvent();
    }else{
        buff[n] = '\0';
        LOG_TRACE<<"onMessage: datalen: " << n;
        onMessage_(buff,shared_from_this());
    }
}

void TcpConnection::handleCloseEvent() {
    LOG_TRACE << "Tcpserver: disconnected ";
    LOG_TRACE << "refcount of channel before removeself:" << this->channel_.use_count();
    kStatus_ = closed;
    this->channel_->removeself();
    LOG_TRACE << "refcount of connection before closecallback  fd: " << sockfd_ << "t his addr " << this;
    LOG_TRACE << "addr of shared_from_this before closecallback  " << shared_from_this();
    LOG_TRACE << "fd of connection before closecallback from shared_from_this " << shared_from_this()->sockfd_;
    closeCallback(shared_from_this());
    //closecallback中 Tcpconnection被析构 后面不能再操作，coredump +未定义操作
    // 函数结束后 actieved中的引用被释放，channel析构

}



