//
// Created by parallels on 5/24/20.
//
#ifdef __APPLE__
#ifndef MUDUO_STUDY_KQUEUE_H
#define MUDUO_STUDY_KQUEUE_H

#include "Poller.h"
#include "log/logger.h"

class kqueue :public Poller{
public:
    explicit kqueue(EventLoop* loop);
    void poller_wait(int timeOut, Channelptrlist* activedChannels) override ;
    void updateChannel(Channelptr) override ;
    void removeChannel(Channelptr) override ;
    bool hasChannel(Channelptr) override ;

    ~kqueue() override;
private:
    void update(uint16_t operation, Poller::Channelptr channel);
    const int kInitEventListSize = 16;
    void fillActiveChannels(int numEvents, Channelptrlist * actived);
    typedef std::vector<struct kevent> EventList;
    EventList change_;
    EventList events_;
    int kqfd_;
    int index_;


};


#endif //MUDUO_STUDY_KQUEUE_H
#endif