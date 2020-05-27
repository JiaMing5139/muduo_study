cmake_minimum_required(VERSION 3.15)
project(muduo_study)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lpthread -O2")
include_directories("./")
add_executable(muduo_study
        Test/main.cpp
        EventLoop.cpp
        EventLoop.h
        base/noncopyable.cpp
        base/noncopyable.h
        log/logger.cpp
        log/AsyncLogging.cpp
        log/appendFile.cpp
        base/currentThread.cpp
        Channel.cpp
        Channel.h
        Poller.cpp
        Poller.h
        Epoll.cpp
        Epoll.h
        base/Types.h
        Timer/Timestamp.cpp Timer/Timestamp.h
        Timer/TimerQueuebase.cpp
        Timer/TimerQueuebase.h
        Timer/TimerQueue.cpp
        Timer/TimerQueue.h
        Timer/Timer.cpp
        Timer/Timer.h
        base/CallBack.h
        EventLoopThread.cpp
        EventLoopThread.h
        SocketsOps.cpp
        SocketsOps.h
        Socket.cpp
        Socket.h
        InetAddress.cpp
        InetAddress.h
        base/Copyable.h
        Acceptor.cpp
        Acceptor.h
        TcpConnection.cpp
        TcpConnection.h
        TcpServer.cpp TcpServer.h
        Buffer.cpp
        Buffer.h
        httpForTest/httpRequest.cpp
        httpForTest/Response.cpp LoopThreadPool.cpp
        LoopThreadPool.h kqueue.cpp kqueue.h
        myeventFd.cpp myeventFd.h Timer/TimerQueuebaseMacos.cpp
        Timer/TimerQueuebaseMacos.h Connector.cpp Connector.h
        TcpClient.cpp
        TcpClient.h
        )
find_package (Threads REQUIRED)

target_link_libraries(muduo_study
        ${CMAKE_THREAD_LIBS_INIT}
        )
