//
// Created by parallels on 5/13/20.
//

#include "TcpConnection.h"
#include "Channel.h"
#include "log/logger.h"
#include "EventLoop.h"
#include <unistd.h>
#include <assert.h>

TcpConnection::TcpConnection(EventLoop *loop,int fd):
sockfd_(fd),
channel_(new Channel(loop,fd),[](Channel * chanptr)
{
    delete chanptr;
    LOG_TRACE<< "Deleter of Channel";
}),
kStatus_(closed),
loop_(loop)
{  // refcount of channel =  1
//    channel_->enableWrite();

    LOG_TRACE << "refcount of channel in ctor of TcpConnection:" << this->channel_.use_count();
}

void TcpConnection::enableRead() {
    assert(kStatus_ == closed);
    kStatus_ = kConnected;
//    channel_->setWriteCallBack(bind(&TcpConnection::handleWriteEvent,shared_from_this()));
//    channel_->setReadCallBack(bind(&TcpConnection::handReadEvent,shared_from_this()));
    channel_->setWriteCallBack(bind(&TcpConnection::handleWriteEvent,this));
    channel_->setReadCallBack(bind(&TcpConnection::handReadEvent,this));
    channel_->setCloseCallBack(bind(&TcpConnection::handleCloseEvent,this));
    LOG_TRACE << "refcount of channel in enable read 1:" << this->channel_.use_count();
    channel_->enableRead();//refcount of channel =  2 channelMap +1
    LOG_TRACE << "refcount of channel in enable read 2:" << this->channel_.use_count();
    assert(onConnection_);
    onConnection_(shared_from_this());

}

void TcpConnection::cancel() {
    if(kStatus_ == kConnected) {
        channel_->removeself();
    }
}

void TcpConnection::handReadEvent() {
    //refcount of channel =  4  | one is in channelMap of Poll, and another is in activedChannels;
    assert(onMessage_);
    if(kStatus_ != closed){
        int savedErrno;
        int n = inputBuffer_.readFd(sockfd_.fd(),&savedErrno);
        if(n == 0){

            handleCloseEvent();
        }else if (n <0){
            if (savedErrno != 11)
                LOG_SYSFATAL <<" readv";
        }else{
            LOG_TRACE<<"Tcpserver: NewConnectionacallback:" <<peerAddr()<<"->"
                     <<localAddr();
            onMessage_(&inputBuffer_,shared_from_this());
        }
    }

}

void TcpConnection::handleCloseEvent() {
    if(kStatus_ == kConnected || kStatus_ == closing) {
        LOG_TRACE << "Tcpserver: disconnected ";
        LOG_TRACE << "refcount of channel before removeself:" << this->channel_.use_count();
        kStatus_ = closed;

        //remove channel in Eventloop
        this->channel_->removeself();//fixme not thread safe
        LOG_TRACE << "refcount of connection before closecallback  fd: " << sockfd_.fd() << "t his addr " << this;
        LOG_TRACE << "addr of shared_from_this before closecallback  " << shared_from_this();
        LOG_TRACE << "fd of connection before closecallback from shared_from_this " << shared_from_this()->sockfd_.fd();
        closeCallback(shared_from_this()); //remove connnection in Tcpserver
        //closecallback中 调用map.eraser TcpConnection 引用计数 - 1 ，仅剩shared_from_this维持的引用计数，函数接受后，参数释放引用计数清零，TcpConnection释放
        // 后面不能再操作，coredump +未定义操作

        // 函数结束后 actieved中的引用被释放，channel析构

        //  LOG_TRACE << "refcount of connection after closecallback  fd: " << sockfd_ << " this addr :" << this; // ok
        //   LOG_TRACE << "refcount of channel accessed with this after closecallback:" << this->channel_.use_count();
//    LOG_TRACE << "fd of connection after closecallback from shared_from_this "<< shared_from_this()->sockfd_; //core dump
    }
}

void TcpConnection::send(const std::string &msg) {
    loop_->runInLoop(bind(&TcpConnection::sendInLoop,shared_from_this(),msg));
}



void TcpConnection::sendInLoop(const std::string &msg) {
       LOG_TRACE << "start to sendInloop";
    LOG_TRACE << "start to sendInloop:" << shared_from_this().use_count();
        ssize_t nwrite;
        if(outputBuffer_.readableBytes() == 0 && !channel_->isWtriting()){
            nwrite = write(sockfd_.fd(),msg.c_str(),msg.length());
            LOG_TRACE << localAddr_ <<"->"<<peerAddr_<<" len:"<< nwrite <<" content:" <<msg;
            if(nwrite >= 0){

                if(nwrite < msg.length()){
                    LOG_TRACE << "write blocked waiting for writeable" <<"total:" <<msg.length() <<" writed:"<<nwrite;
                    outputBuffer_.append(msg.c_str() + nwrite,msg.length() - nwrite);
                    LOG_TRACE << "outpubuffer size:"<< outputBuffer_.readableBytes();
                    channel_->enableWrite();
                }

            }else{
                if(errno != EAGAIN && errno != 32){
                    LOG_SYSFATAL <<"write";
                }
            }

        }else{
             outputBuffer_.append(msg.c_str() ,msg.length());
        }

}

void TcpConnection::handleWriteEvent() {
        LOG_TRACE<<"handleWriteEvent<< current status: " << kStatus_;
        if(kStatus_ == closing){
            if(outputBuffer_.readableBytes() == 0 ){
                assert(channel_->isWtriting());
                channel_->disableWriting();
                shutdownInLoop();

            }else{
                std::string msg =  outputBuffer_.retrieveAllAsString();
                int nwrite = write(sockfd_.fd(),msg.c_str(),msg.length());
                if(nwrite >= 0) {
                    if (nwrite < msg.length()) {
                        LOG_TRACE << "write blocked" << std::endl;
                        outputBuffer_.append(msg.c_str() + nwrite, msg.length() - nwrite);
                        LOG_TRACE << "outpubuffer size:"<< outputBuffer_.readableBytes();
                        assert(channel_->isWtriting());
                    }else{
                        assert(channel_->isWtriting());
                        channel_->disableWriting();
                        shutdownInLoop();
                    }
                } else{
                    if(errno != EAGAIN){
                        LOG_SYSFATAL <<"write";
                    }
                }
            }

        }

        if(kStatus_ == kConnected){
            if(outputBuffer_.readableBytes() == 0 ){
                LOG_TRACE << "send over1";
                assert(channel_->isWtriting());
                channel_->disableWriting();
            }else{
                std::string msg =  outputBuffer_.retrieveAllAsString();
                int nwrite = write(sockfd_.fd(),msg.c_str(),msg.length());
                if(nwrite >= 0) {
                    if (nwrite < msg.length()) {
                        LOG_TRACE << "write blocked waiting for writeable" <<"total:" <<msg.length() <<" writed:"<<nwrite;
                        outputBuffer_.append(msg.c_str() + nwrite, msg.length() - nwrite);
                        LOG_TRACE << "outpubuffer size:"<< outputBuffer_.readableBytes();
                        assert(channel_->isWtriting());
                    }else{
                        LOG_TRACE << "send over2";
                        assert(channel_->isWtriting());
                        channel_->disableWriting();
                    }
                } else{
                    if(errno != EAGAIN){
                        LOG_SYSFATAL <<"write";
                    }
                }
            }

        }


}
void TcpConnection::shutdown() {
    if(kStatus_ == kConnected){
        kStatus_ = closing;
        loop_->runInLoop(bind(&TcpConnection::shutdownInLoop,shared_from_this()));
    }
}

void TcpConnection::shutdownInLoop() {

    loop_->assertInLoopThread();
    if (!channel_->isWtriting())
    {
        LOG_TRACE << "try to shutdown";
        sockfd_.shutdownWrite();
    }
}



