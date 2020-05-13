//
// Created by parallels on 5/12/20.
//

#include "InetAddress.h"
#include <string.h>
#include "SocketsOps.h"
InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6) {
    if(ipv6){

    }else{

        bzero(&addr_,sizeof addr_);
        addr_.sin_family=AF_INET;
        addr_.sin_port = htobe16(port);
        addr_.sin_addr.s_addr = INADDR_ANY;
    }

}

InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6) {

    if(ipv6){

    }else{

    }


}

void InetAddress::setSockAddrInet6(const struct sockaddr_in6 &addr) {
        this->addr6_ = addr;
}

struct sockaddr *InetAddress::getSockaddr() {
   return sockets::sockaddr_cast(&this->addr6_);
}
