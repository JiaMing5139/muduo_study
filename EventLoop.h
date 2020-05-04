#ifndef MUDUO_STUDY_EVENTLOOP_H
#define MUDUO_STUDY_EVENTLOOP_H

#include "base/noncopyable.h"
#include <cstdlib>
#include <memory>
#include <vector>
#include <atomic>

class Poller;
class Channel;
class EventLoop: noncopyable{
public:
    EventLoop();
    typedef std::shared_ptr<Channel> Channelptr;
    typedef std::vector<Channelptr> Channelptrlist;
    void assertInLoopThread(){
        if(!runInthread()){
            abortInthread();
        }
    }
    ~EventLoop();
    void loop();
    void update(Channelptr channel);
    void quit();

private:
    bool runInthread();
    void abortInthread();

    const pid_t threadId_;

    const std::unique_ptr<Poller> poll_; // can't be moved ,replace scoped_ptr
    Channelptrlist activedChannls;

    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
};


#endif