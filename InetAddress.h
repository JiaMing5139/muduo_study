//
// Created by parallels on 5/12/20.
//

#ifndef MUDUO_STUDY_INETADDRESS_H
#define MUDUO_STUDY_INETADDRESS_H

#include "base/Copyable.h"
#include <string>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>

class InetAddress: public Copyable {
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);


    InetAddress(std::string ip, uint16_t port, bool ipv6 = false);

    explicit InetAddress(const struct sockaddr_in& addr)
            : addr_(addr)
    { }

    explicit InetAddress(const struct sockaddr_in6& addr)
            : addr6_(addr)
    { }

    void setSockAddrInet6(const struct sockaddr_in6& addr);

    struct sockaddr* getSockaddr();

private:
    union
    {
        struct sockaddr_in addr_;
        struct sockaddr_in6 addr6_;
    };

};


#endif //MUDUO_STUDY_INETADDRESS_H
