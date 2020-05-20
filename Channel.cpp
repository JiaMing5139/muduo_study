//
// Created by parallels on 4/29/20.
//

#include "Channel.h"
#include "EventLoop.h"
#include <sys/poll.h>
#include "log/logger.h"
#include <assert.h>
#include <unistd.h>
const int Channel::NoneEvent = 0;
const int Channel::ReadEvent = POLLIN | POLLPRI;
const int Channel::WriteEvent = POLLOUT;


Channel::Channel(EventLoop *loop, int fd) :
fd_(fd),
index_(-1),
loop_(loop),
revents_(0),
events_(0) {
    LOG_TRACE<<" genernate a new Channel";
}

void Channel::handleEvent() {
    if(revents_ & POLLHUP){
        LOG_TRACE<<"EVENT POLLUP:" <<" disconnected!";
    }

    if(revents_&POLLNVAL){
        LOG_TRACE<<"WARN:" <<"POLLNVAL";
    }

    if(revents_ & (POLLIN|POLLPRI|POLLHUP)) {
        assert(readEventCallback);
        readEventCallback(1);
    }


    if(revents_ & POLLOUT) {
        assert(writeEventCallback);
        writeEventCallback();
    }
    if(revents_ & (POLLERR|POLLNVAL)) {
        assert(errorCallBack);
        errorCallBack();
    }

}

void Channel::update() {
    loop_->update(shared_from_this());
}

void Channel::removeself() {

    loop_->cancel(shared_from_this());

}

Channel::~Channel() {
   int ret =  close(fd_);
   if(ret == -1)
   LOG_SYSFATAL << " ~Channel close fd" ;

}
