//
// Created by parallels on 5/12/20.
//

#include "InetAddress.h"
#include <string.h>
#include "SocketsOps.h"
InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6) :
        ipstr_("any_addr") ,
        portstr_(port)
{
    if(ipv6){

    }else{

        bzero(&addr_,sizeof addr_);
        addr_.sin_family=AF_INET;
        addr_.sin_port = htobe16(port);
        addr_.sin_addr.s_addr = INADDR_ANY;
    }

}

InetAddress::InetAddress(std::string ip, uint16_t port, bool ipv6):
ipstr_(std::move(ip)) ,
portstr_(port)
{

    if(ipv6){

    }else{
        sockets::fromIpPort(ip.c_str(),port,&addr_);
    }

}

void InetAddress::setSockAddrInet6(const struct sockaddr_in6 &addr) {
        this->addr6_ = addr;
}

struct sockaddr *InetAddress::getSockaddr() {
   return sockets::sockaddr_cast(&this->addr6_);
}

std::ostream &operator<<(std::ostream &os, const InetAddress &inetAddress) {
    return os << inetAddress.ipstr_ << ":" << inetAddress.portstr_;
}