//
// Created by parallels on 5/25/20.
//

#ifndef MUDUO_STUDY_CHATSERVER_H
#define MUDUO_STUDY_CHATSERVER_H

#include "TcpServer.h"
#include "Codec.h"
#include "DisPatcher.h"
#include "ChatMessage.pb.h"
class EventLoop;
class ChatServer {
public:
    enum status{
        closed,
        running
    };
    ChatServer(EventLoop *, const InetAddress &addr);

    void start();

    void onConnection(TcpConnectionPtr conn);
    void onMessage(Buffer * buff, TcpConnectionPtr conn);
    void onProtobufMessage(Codec::MessagePtr message, std::shared_ptr<TcpConnection> connptr);
    void onProtobufErroMessage(Buffer *, std::shared_ptr<TcpConnection> connptr);

    void handelMessageChat( DisPatcher::Messageptr msg);
    explicit  ChatServer(const InetAddress &addr);

private:

    std::atomic<status> status_;
    EventLoop * baseloop_;
    TcpServer server_;
    Codec codec_;
    DisPatcher disPatcher_;
    typedef  std::weak_ptr<TcpConnection> TcpConnectionWeakPtr ;
    typedef std::set<TcpConnectionPtr> TcpConnWeakPtrs;

    TcpConnWeakPtrs tcpConnWeakPtrs_;





};


#endif //MUDUO_STUDY_CHATSERVER_H
