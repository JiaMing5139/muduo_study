//
// Created by parallels on 5/24/20.
//


#ifdef __APPLE__
#include "kqueue.h"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <poll.h>
namespace {
    const int kNew = -1;
    const int kDeleted = 0;
    const int kAdded = 1;
}
int createKq(){
    int kq = ::kqueue();
    if(kq == -1){
        LOG_SYSFATAL<<"kqueue";
    }
    return kq;
}
kqueue::kqueue(EventLoop *loop) :
Poller(loop),
kqfd_(createKq()),
index_(0),
change_(kInitEventListSize),
events_(kInitEventListSize)
{

}

void kqueue::poller_wait(int timeOut, Poller::Channelptrlist *activedChannels) {
    timespec it = {timeOut,0};
    int numEvents =kevent(kqfd_, &*change_.begin(),index_, &*events_.begin(),16,NULL);
    if (numEvents > 0) {
        LOG_TRACE << "kqueue: kevent:" <<numEvents << " happened" ;
        if(numEvents == events_.size()){
            events_.resize(events_.size() * 2);
        }
        fillActiveChannels( numEvents,  activedChannels);

    } else if(numEvents == 0){
        // LOG_TRACE << "noting happened" ;
    }else{
        if(errno != EINTR){
            perror("kqueue");
            LOG_SYSFATAL << "kevent";
            abort();
        }
    }

}

void kqueue::updateChannel(Poller::Channelptr channel) {
    assertInLoopThread();
    //add

    if(channel->index() == kNew || channel->index() == kDeleted){

        update(EV_ADD, channel);
        channel->setIndex(kAdded);
        Poller::channels_[channel->fd()] = channel;

#ifdef  NDEBUG
        LOG_TRACE << "fd:"<< Poller::channels_[channel->fd()]->fd()<< " has been added";
#endif


    }else{

        update(EV_ADD, channel);

    }
    //update

}

void kqueue::removeChannel(Poller::Channelptr channel) {
    assertInLoopThread();
    assert(channel->index() == kAdded);
    if(channel->index() == kAdded){
        update(EV_DELETE, channel);
        channel->setIndex(kDeleted);
        channels_.erase(channel->fd());
    }
}

bool kqueue::hasChannel(Poller::Channelptr) {
    return false;
}

kqueue::~kqueue() {

}

int kEventstoPevents(int  flag){
    int retFlag = 0;
    if(flag == EVFILT_READ)
    {
        retFlag |= POLLIN;
    }

    if(flag == EVFILT_WRITE)
       retFlag |= POLLOUT;
     return retFlag;
}

int PeventstokEvents(int flag ){
    int retFlag = 0;
    if(flag &  POLLIN)
        retFlag = EVFILT_READ;
    if(flag &  POLLOUT)
       retFlag =EVFILT_WRITE ;
    return retFlag;

}

void kqueue::update(uint16_t operation, Poller::Channelptr channel) {
    struct kevent ev{};
    bzero(&ev,sizeof ev);;
    int events = PeventstokEvents(channel->events()) ;
    LOG_TRACE << " EV_SET operation = " << operation  << " events " << events <<" fd:" << channel->fd() <<" added pos:" << index_;
    if(index_ == change_.size()){
        change_.resize(events_.size() * 2);
    }
    EV_SET(&*change_.begin(), channel->fd(), events, operation | EV_ENABLE, 0, 0, NULL);
    if (-1 == kevent(kqfd_, &*change_.begin(), 1, NULL, 0, NULL)){
        LOG_SYSFATAL<<"EV_SET";
    }
}



void kqueue::fillActiveChannels(int numEvents, Poller::Channelptrlist *actived) {
    for(int index = 0 ; index < numEvents ; index ++){

        Channelptr channel =  Poller::channels_[(int)events_[index].ident];
        if(channel){

            int flag  = events_[index].filter ;

            int pevent = kEventstoPevents(flag);
            LOG_TRACE << "activdchannel fd found:" << channel->fd() << " kevents:" <<  events_[index].filter <<" pollevents:" << pevent;
            channel->setRevents(pevent);
            actived->push_back(channel);
        }else{
           LOG_SYSFATAL<<"not found channel";
        }

    }
}


#endif


