//
// Created by parallels on 5/4/20.
//

#ifndef MUDUO_STUDY_TIMERQUEUEBASE_H
#define MUDUO_STUDY_TIMERQUEUEBASE_H


#include <vector>
#include "Timer.h"
#include <set>
#include <memory>
class Timestamp;
class Channel;
class EventLoop;

class TimerQueuebase {
    typedef std::shared_ptr<Channel> Channelptr;
    typedef std::pair<Timestamp,std::shared_ptr<Timer>>  entry;
    typedef std::set<entry> TimerList;
    typedef std::vector<entry> TimerVec;
public:
    explicit TimerQueuebase(EventLoop * loop);
    void addTimer(Timestamp timestamp,Timer::TimerCallback cb,double interval);
    void cancel();

    void addTimerInLoop(std::shared_ptr<Timer> timer);
private:



    bool insert(std::shared_ptr<Timer> timer);
    void handleEvent();
    void reset(Timestamp timestamp);
    TimerList timerList_;
    int timerFd_;
    Channelptr channel_;
    EventLoop * loop_;
    TimerVec activedTimerList_;

    int createTimerFd();
};


#endif //MUDUO_STUDY_TIMERQUEUEBASE_H
