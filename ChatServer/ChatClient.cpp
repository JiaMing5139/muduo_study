//
// Created by parallels on 5/25/20.
//

#include "ChatClient.h"
#include "Buffer.h"
#include "EventLoop.h"
#include "log/logger.h"
#include <google/protobuf/message.h>
#include "ChatMessage.pb.h"
#include <iostream>
#include <zconf.h>



ChatClient::ChatClient(EventLoop *loop, const InetAddress &addr):
baseloop_(loop),
client_(baseloop_,addr),
status_(closed),
sequenceNum_(0),
inputChannel_(new Channel(baseloop_,STDIN_FILENO))
{

}

ChatClient::ChatClient(const InetAddress &addr):
baseloop_(new EventLoop),
client_(baseloop_,addr),
status_(closed),
sequenceNum_(0),
inputChannel_(new Channel(baseloop_,STDIN_FILENO))
{

}



void ChatClient::onMessage(Buffer *buff, TcpConnectionPtr conn) {
    std::cout<< buff->retrieveAllAsString() << std::endl;
}

void ChatClient::start() {
    if(status_ == closed){
        status_ = running;

        inputChannel_->setReadCallBack(std::bind(&ChatClient::onStdinRead,this));
        inputChannel_->setWriteCallBack(std::bind(&ChatClient::onStdinWrite,this));
        inputChannel_->enableRead();

        client_.setOnConnectionCallback(bind(&ChatClient::onConnection,this,std::placeholders::_1));
        client_.setOnMessageCallback(bind(&ChatClient::onMessage,this,std::placeholders::_1,std::placeholders::_2));
        client_.start();
        baseloop_->loop();
    }
}

void ChatClient::onStdinWrite() {

}

void ChatClient::onStdinRead() {
    char buff[2048] ={0};
    Jimmy::MessageChat messge;


    int nread = read(STDIN_FILENO,buff,sizeof buff);
    if(nread < 0){
        LOG_SYSFATAL << "read";
    }

    messge.set_sequence(sequenceNum_ ++);
    messge.set_datapayload(buff,nread);
    int totalen = 0;
    char protoBuff[4096] ={0};

    codec_.FillBuffer(messge,protoBuff,&totalen);

    std::string msg(protoBuff ,totalen + 4);

    client_.send(msg);


}

void ChatClient::onConnection(TcpConnectionPtr conn) {

}
