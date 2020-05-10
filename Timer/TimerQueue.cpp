//
// Created by parallels on 5/4/20.
//

#include "TimerQueue.h"
#include "TimerQueuebase.h"
void TimerQueue::addTimer(Timestamp timestamp,Timer::TimerCallback cb,double interval) {
    timerimpl_->addTimer(timestamp,cb,interval);
}

void TimerQueue::cancel() {
    timerimpl_->cancel();
}

TimerQueue::TimerQueue(EventLoop * loop):timerimpl_(new TimerQueuebase(loop)) {

}

TimerQueue::~TimerQueue() {
    delete timerimpl_;
}
