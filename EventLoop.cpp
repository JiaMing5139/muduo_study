
#include "EventLoop.h"
#include "base/currentThread.h"
#include "log/logger.h"
#include "Epoll.h"
__thread EventLoop * loop_ = 0 ;
EventLoop::EventLoop() :
threadId_(Jimmy::CurrentThread::tid()),
poll_(new Epoll(this)),
looping_(false),
quit_(true)
{
    LOG_TRACE << "genernate a new loop in thread::" << threadId_;
    if(loop_ ){
        LOG_TRACE << "Fatal error: genernate a new loop in thread which alread has a loop::" << threadId_;
        abort();
    }else{
        loop_ = this;
    }
}

bool EventLoop::runInthread() {
    return Jimmy::CurrentThread::tid() == threadId_;
}

void EventLoop::loop() {
    assertInLoopThread();
    activedChannls.clear();
    LOG_TRACE << "start a loop";
    looping_ = true;
    quit_ = false;

    while(!quit_){
        poll_->poller_wait(-1,&activedChannls);
        LOG_TRACE << "actived Channels size" << activedChannls.size();
        for(const auto channel: activedChannls){
            channel->handleEvent();
        }
    }

    looping_ = false;


}

void EventLoop::abortInthread() {
    LOG_TRACE << "Not running in IO thread";
    abort();
}

void EventLoop::update(EventLoop::Channelptr channel) {
    poll_->updateChannel(channel);

}

EventLoop::~EventLoop() {

}

void EventLoop::quit() {
   quit_ = true;
}

