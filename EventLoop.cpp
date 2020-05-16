
#include "EventLoop.h"
#include "base/currentThread.h"
#include "log/logger.h"
#include "Epoll.h"
#include <sys/eventfd.h>
#include <assert.h>
__thread EventLoop * loop_ = 0 ;
int createEventfd(){
    int ret = eventfd(0,EFD_NONBLOCK);
    if (ret == -1){
        perror("eventfd_create");
        abort();
    }
}

void readEventfd(int fd){
    eventfd_t value;
    int ret = eventfd_read(fd, &value);
    if (ret == -1){
        perror("eventfd_read");
        abort();
    }
}

EventLoop::EventLoop() :
        threadId_(Jimmy::CurrentThread::tid()),
        poll_(new Epoll(this)),
        looping_(false),
        quit_(true),
        timerQueue(this),
        wakeUpfd_(createEventfd()),
        wakeupChannel_(new Channel( this,wakeUpfd_)),
        doingFunctors(false),
        callingPendingFunctors_(false)
        {
    LOG_TRACE << "genernate a new loop in thread::" << threadId_;
    if(loop_ ){
        LOG_TRACE << "Fatal error: genernate a new loop in thread which alread has a loop::" << threadId_;
        abort();
    }else{
        loop_ = this;
    }

    wakeupChannel_->setReadCallBack(std::bind(readEventfd, wakeUpfd_)); //s
    wakeupChannel_->enableRead();
}

bool EventLoop::runInthread() {
    return Jimmy::CurrentThread::tid() == threadId_;
}

void EventLoop::loop() {
    assertInLoopThread();
    LOG_TRACE << "start a loop";
    looping_ = true;
    quit_ = false;

    while(!quit_){
        activedChannls.clear();
        poll_->poller_wait(-1,&activedChannls);
        LOG_TRACE << "actived Channels size:" << activedChannls.size();
        for(const auto &channel: activedChannls){
            channel->handleEvent();
        }

        doPendingFunctors();
    }

    looping_ = false;


}

void EventLoop::abortInthread() {
    LOG_TRACE << "Not running in IO thread";
    abort();
}

void EventLoop::update(EventLoop::Channelptr channel) {
    assert(channel->fd() >= 0);
    poll_->updateChannel(channel);
}

void EventLoop::cancel(EventLoop::Channelptr channel) {
    assertInLoopThread();
    poll_->removeChannel(channel);
}

EventLoop::~EventLoop() {

}

void EventLoop::quit() {
    if(quit_== false){
        quit_ = true;
        wakeup();
    }
}

void EventLoop::runAt(Timestamp timerstamp, TimerCallback cb) {
    timerQueue.addTimer(timerstamp,cb,0);

}

void EventLoop::runAfter(double delay, TimerCallback cb) {
    Timestamp timestamp(addTime(Timestamp::now(),delay));
    timerQueue.addTimer(timestamp,cb,0);
}

void EventLoop::runEvery(double interval, TimerCallback cb) {
    Timestamp timestamp(addTime(Timestamp::now(),interval));
    timerQueue.addTimer(timestamp,cb,interval);
}

void EventLoop::runInLoop(funcCallback cb) {
    if(runInthread()){
        LOG_TRACE << "runInLoop was already in current loop" ;
        cb();
    }else{
        queueInloop(std::move(cb));
    }


}

//被runinloop调用，只会在非loop线程中被调用
void EventLoop::queueInloop(funcCallback cb) {
    LOG_TRACE << "try to add a new functor in loop" ;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        funcList_.push_back(std::move(cb));
    }

   // 1.若在runInloop中调用调用queueInloop !runInthread()必定为true
   // 2.若在IO线程外直接调用queueInloop，则需要在分析在callback中再次调用queueInloop，
   // 即在当前线程中再次直接调用queueloop，这时需要进行callingPendingFunctors_的判断
    if (!runInthread() || callingPendingFunctors_)
        //让一批functor一次执行完毕
    //并不是每次都需要唤醒eventloop 理解为一批新加入的functor调用一次wakeup，什么时候换到下一批？ funclist被交换成空的之后
    if(!doingFunctors) {
        LOG_TRACE << " try to WAKE UP ,because of new funcotr" ;
        wakeup();
        doingFunctors = true; //protected by atomic
    }



}


void EventLoop::doPendingFunctors() {
    std::vector<funcCallback> tmp_funlist;
    callingPendingFunctors_ =true;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        tmp_funlist.swap(funcList_);
        doingFunctors = false;
    }

    LOG_TRACE << "doPendingFunctors: try to call callback functorlist size:" << tmp_funlist.size();
    for(const auto &f:tmp_funlist){
        f();
    }



    tmp_funlist.clear();
    callingPendingFunctors_ =false;

}

void EventLoop::wakeup() {
    int ret =  eventfd_write(wakeUpfd_,1);
    if (ret == -1){
        perror("eventfd_write");
        abort();
    }
}



