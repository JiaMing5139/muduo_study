cmake_minimum_required(VERSION 3.15)
project(muduo_study)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lpthread")

add_executable(muduo_study
        main.cpp
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
        )
