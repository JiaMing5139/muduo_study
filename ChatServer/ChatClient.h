//
// Created by parallels on 5/25/20.
//

#ifndef MUDUO_STUDY_CHATCLIENT_H
#define MUDUO_STUDY_CHATCLIENT_H

#include "TcpClient.h"
#include "Codec.h"
#include "DisPatcher.h"
#include "Channel.h"
#include <atomic>
class EventLoop;
class ChatClient {
public:
    enum status{
        closed,
        running
    };

    explicit ChatClient(const InetAddress &addr);
    ChatClient(EventLoop * loop, const InetAddress &addr);
    void start();

    void onConnection(TcpConnectionPtr conn);
    void onMessage(Buffer * buff, TcpConnectionPtr conn);

    void onStdinWrite();
    void onStdinRead();

private:
    std::atomic<int> sequenceNum_ ;
    Codec codec_;
    DisPatcher disPatcher_;
    EventLoop * baseloop_;
    std::shared_ptr<Channel> inputChannel_;
    std::atomic<status> status_ ;
    TcpClient client_;

};


#endif //MUDUO_STUDY_CHATCLIENT_H
