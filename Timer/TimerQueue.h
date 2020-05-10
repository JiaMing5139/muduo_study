//
// Created by parallels on 5/4/20.
//

#ifndef MUDUO_STUDY_TIMERQUEUE_H
#define MUDUO_STUDY_TIMERQUEUE_H
#include "../base/noncopyable.h"
#include "Timer.h"
class Timestamp;
class TimerQueuebase;
class EventLoop;

class TimerQueue: public noncopyable {
public:
    explicit TimerQueue(EventLoop*);
    ~TimerQueue();
    void addTimer(Timestamp timestamp,Timer::TimerCallback cb,double interval);
    void cancel();


private:
    TimerQueuebase * timerimpl_;
};


#endif //MUDUO_STUDY_TIMERQUEUE_H
