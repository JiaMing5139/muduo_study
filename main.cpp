#include <iostream>
#include "log/logger.h"
#include "EventLoop.h"
#include <assert.h>
#include <thread>
#include "Epoll.h"
#include <sys/socket.h>
#include <memory>
#include <sys/timerfd.h>
#include <string.h>
#include "log/AsyncLogging.h"
#include "log/appendFile.h"

#include "Timer/TimerQueue.h"
#include <unistd.h>


/**  Test Asynlog  **/

std::unique_ptr<AsyncLogging> asyn(new AsyncLogging("timerqueue"));
void g_output(const char * msg,size_t len){
    asyn->append(msg,len);
}

void setAsynLog(){
    Jimmy::Logger::setOutput(g_output);
}

void loggerTest(){
    asyn->start();
    setAsynLog();
    LOG_TRACE << "testLog";
}

/**  Test Reactor  **/
EventLoop* loop ;

void timeout(int fd){
    printf("timeout\n");
}

void testEventLoop(){
    loop= new EventLoop;
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);

    howlong.it_value.tv_sec = 10;
    timerfd_settime(timerfd,0,&howlong,NULL);
    std::shared_ptr<Channel> channelptr (new Channel(loop,timerfd));
    channelptr->setReadCallBack(std::bind(timeout, timerfd));
    channelptr->enableRead();
    loop->loop();
}

void testTimerQueue(){
    // loggerTest();
    loop= new EventLoop;
    TimerQueue base(loop);
    Timestamp timestamp(Timestamp::now());

    base.addTimer( addTime(timestamp,1.0),std::bind(timeout, 1),0);
    base.addTimer( addTime(timestamp,1.0),std::bind(timeout, 1),0);
    base.addTimer( addTime(timestamp,1.0),std::bind(timeout, 1),0);
    base.addTimer( addTime(timestamp,3.0),std::bind(timeout, 1),0);
//    base.addTimer( addTime(timestamp,5.0),timeout,0);
//    base.addTimer( addTime(timestamp,5.0),timeout,0);
    loop->loop();

}



int main() {
    //loggerTest();
    //testEventLoop();
    testTimerQueue();

}