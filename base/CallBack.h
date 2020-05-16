//
// Created by parallels on 5/11/20.
//

#ifndef MUDUO_STUDY_CALLBACK_H
#define MUDUO_STUDY_CALLBACK_H

#include <functional>
#include <memory>
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void ()> funcCallback;
typedef std::function<void ()> TimerCallback;


typedef std::function<void (const TcpConnectionPtr &conn)> TcpEventCallback;
typedef std::function<void (const char *,TcpConnectionPtr conn)> readTcpEventCallback;
#endif //MUDUO_STUDY_CALLBACK_H