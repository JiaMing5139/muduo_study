//
// Created by parallels on 5/6/20.
//

#include "Timestamp.h"

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
