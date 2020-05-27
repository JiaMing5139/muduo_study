//
// Created by parallels on 5/25/20.
//

#include "ChatServer.h"
#include "log/logger.h"
ChatServer::ChatServer(EventLoop * loop,const InetAddress &addr) :
server_(addr,loop),
status_(closed)
{

}
ChatServer::ChatServer(const InetAddress &addr) :
baseloop_(new EventLoop),
server_(addr,baseloop_),
status_(closed)
{
}

void ChatServer::start() {
    if(status_ == closed){
        status_ = running;
        server_.setOnConnectionCallback(bind(&ChatServer::onConnection,this,std::placeholders::_1));
        server_.setOnMessageCallback(bind(&Codec::onMessage,&codec_,std::placeholders::_1,std::placeholders::_2));

        codec_.setOnMessageCompleteCallback(std::bind(&ChatServer::onProtobufMessage,this,std::placeholders::_1,std::placeholders::_2));
        codec_.setOnErrorCallback(std::bind(&ChatServer::onProtobufErroMessage,this,std::placeholders::_1,std::placeholders::_2));


        disPatcher_.registerCallback(Jimmy::MessageChat::GetDescriptor()->name(),bind(&ChatServer::handelMessageChat,this,std::placeholders::_1));

        server_.start();
        baseloop_->loop();
    }

}

void ChatServer::onConnection(TcpConnectionPtr conn) {

    tcpConnWeakPtrs_.insert(conn);
    LOG_TRACE << "onConnection:" << conn->peerAddr() ;
}

void ChatServer::onMessage(Buffer *buff, TcpConnectionPtr conn) {


}

void ChatServer::onProtobufMessage(Codec::MessagePtr message, std::shared_ptr<TcpConnection> connptr) {
     std::shared_ptr<Jimmy::MessageChat> mss =  std::static_pointer_cast<Jimmy::MessageChat>(message);
     LOG_TRACE << mss->DebugString();
     disPatcher_.onMessage(message);
}


void ChatServer::onProtobufErroMessage(Buffer * buff, std::shared_ptr<TcpConnection> connptr) {

}

void ChatServer::handelMessageChat( DisPatcher::Messageptr msg) {
    LOG_TRACE <<"handle MessageChat" ;
    std::shared_ptr<Jimmy::MessageChat> chatMessage =   std::static_pointer_cast<Jimmy::MessageChat>(msg);
    for(const auto & conn : tcpConnWeakPtrs_){
        conn->send(chatMessage->datapayload());
        }
}




