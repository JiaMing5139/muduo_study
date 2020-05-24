//
// Created by jiaming pan on 5/24/20.
//

#include "myeventFd.h"
#include <unistd.h>
#include "log/logger.h"

#ifdef __linux__
#include <sys/eventfd.h>
#endif

int Jimmy::myeventFd::eventfd_read() {
    int value;
    int ret = read(readfd_,&value,sizeof value);
    if(ret == -1){
        LOG_SYSFATAL <<"write";
    }
}

int Jimmy::myeventFd::fd() {
    return readfd_;
}

int Jimmy::myeventFd::eventfd_write() {
    int value = 1;
    int ret = write(fds[1],&value,sizeof value);
    if(ret == -1){
        LOG_SYSFATAL <<"write";
    }
    return 0;
}

Jimmy::myeventFd::myeventFd():
readfd_(createFd())
{
}

int Jimmy::myeventFd::createFd() {
#ifdef __linux__
    int ret = eventfd(0,EFD_NONBLOCK);
    if (ret == -1){
      LOG_SYSFATAL <<"eventfd";
    }

    return ret;
#endif

#ifdef __APPLE__
    int ret = pipe(fds);
    if(ret == -1){
        LOG_SYSFATAL <<"pipe";
    }
    return fds[0];
#endif
}

Jimmy::myeventFd::~myeventFd() {
#ifdef __APPLE__
    close(fds[0]);
    close(fds[1]);
#endif

#ifdef __linux__
    close readfd_;
#endif
}

