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

#include <unistd.h>


/**  Test Asynlog  **/

std::unique_ptr<AsyncLogging> asyn(new AsyncLogging("test"));
void g_output(const char * msg,size_t len){
    asyn->append(msg,len);
}

void setAsynLog(){
    Jimmy::Logger::setOutput(g_output);
}

void loggerTest(){
    setAsynLog();
    LOG_TRACE << "testLog";
}

/**  Test Reactor  **/
EventLoop* loop ;

void timeout(){
    printf("timeout\n");
    loop->quit();
}

void testEventLoop(){
    loop= new EventLoop;
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);

    howlong.it_value.tv_sec = 5;
    timerfd_settime(timerfd,0,&howlong,NULL);
    std::shared_ptr<Channel> channelptr (new Channel(loop,timerfd));
    channelptr->setReadCallBack(timeout);
    channelptr->enableRead();
    loop->loop();
}



int main() {
    testEventLoop();


}