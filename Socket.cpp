//
// Created by parallels on 5/12/20.
//

#include "Socket.h"
#include "SocketsOps.h"
#include <string.h>
#include "log/logger.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
void Socket::bindAddress(InetAddress addr) {
    sockets::bindOrDie(socketfd_,addr.getSockaddr());
}

void Socket::listen() {
    sockets::listenOrDie(this->socketfd_);
}

void Socket::connect() {

}

int Socket::accept(InetAddress *peerAddr) {
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(socketfd_, &addr);
    if (connfd >= 0)
    {
        peerAddr->setSockAddrInet6(addr);
    }
    return connfd;

}


void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    if(setsockopt(socketfd_, IPPROTO_TCP, TCP_NODELAY,
                 &optval, static_cast<socklen_t>(sizeof optval)) < 0){
        LOG_SYSFATAL<<"setsockopt nodelay";
    }

}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    if(setsockopt(socketfd_, SOL_SOCKET, SO_REUSEADDR,
                  &optval, static_cast<socklen_t>(sizeof optval)) < 0){
        LOG_SYSFATAL<<"setsockopt setReuseaddr";
    }

}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    if(setsockopt(socketfd_, SOL_SOCKET, SO_REUSEPORT,
                  &optval, static_cast<socklen_t>(sizeof optval)) < 0){
        LOG_SYSFATAL<<"setsockopt setReusePort";
    }
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    if(setsockopt(socketfd_, IPPROTO_TCP, SO_KEEPALIVE,
                  &optval, static_cast<socklen_t>(sizeof optval)) < 0){
        LOG_SYSFATAL<<"setsockopt nodelay";
    }

}

void Socket::shutdownWrite() {

}

Socket::Socket() : socketfd_(sockets::createNonblockingOrDie(AF_INET)) {

}

Socket::Socket(int fd) : socketfd_(fd) {

}
