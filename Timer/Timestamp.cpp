//
// Created by parallels on 5/6/20.
//

#include "Timestamp.h"
#include <string.h>


__thread time_t cache_time = 0;
__thread struct tm  cached_locoaltimeStruct;


Timestamp::Timestamp(int64_t microTime):mircotime_(microTime) {

}

bool operator<(const Timestamp &lhs, const Timestamp &rhs) {
    return lhs.SecondsSinceEpoch() < rhs.SecondsSinceEpoch();
}

bool operator==(const Timestamp &lhs, const Timestamp &rhs) {
    return lhs.SecondsSinceEpoch() == rhs.SecondsSinceEpoch();
}

std::ostream &operator<<(std::ostream & os, const Timestamp & timestamp) {

    time_t seconds = timestamp.SecondsSinceEpoch();
    auto locoaltimeStruct = localtime(&seconds);
    std::string time_s(asctime(locoaltimeStruct));
    return os << time_s;
}

int64_t Timestamp::cachedtime() {
    if(cache_time == 0 ){
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        time_t now_seconds = tv.tv_sec;
        cache_time = now_seconds;
    }
    return cache_time;
}

struct tm * Timestamp::timetoStruct() {
    if(cache_time == 0){   //第一次调用 初始化cached time和struct
        time_t first  = cachedtime();
        const time_t *time   = &first;
        memcpy(&cached_locoaltimeStruct,localtime(time), sizeof (struct tm));
    }else{
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        time_t now_seconds = tv.tv_sec;
        if(now_seconds - cachedtime() >= 3){
            const time_t *time =  &now_seconds;
            cache_time = now_seconds;
            memcpy(&cached_locoaltimeStruct,localtime(time), sizeof (struct tm));
        }else{
            cached_locoaltimeStruct.tm_sec +=  now_seconds - cachedtime();
        }
    }
    return &cached_locoaltimeStruct;

}

