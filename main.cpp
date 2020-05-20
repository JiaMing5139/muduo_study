#include <iostream>
#include "log/logger.h"
#include "EventLoop.h"
#include <assert.h>
#include <thread>
#include "Epoll.h"
#include <sys/socket.h>
#include <memory>
#include <sys/timerfd.h>
#include <string.h>
#include "log/AsyncLogging.h"
#include "log/appendFile.h"
#include "EventLoopThread.h"
#include "Timer/TimerQueue.h"
#include "base/currentThread.h"
#include <unistd.h>
#include "Socket.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "Buffer.h"



/**  Test Asynlog  **/

std::unique_ptr<AsyncLogging> asyn(new AsyncLogging("timerqueue"));
void g_output(const char * msg,size_t len){
    asyn->append(msg,len);
}

void setAsynLog(){
    Jimmy::Logger::setOutput(g_output);
    asyn->start();
}

void loggerTest() {
    setAsynLog();
    LOG_TRACE << "testLog";
}


/**  Test Reactor  **/
EventLoop* loop ;

void timeout(int fd){
    printf("timeout\n");
}

void testEventLoop(){
    loop= new EventLoop;
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 2;
    timerfd_settime(timerfd,0,&howlong,NULL);
    std::shared_ptr<Channel> channelptr (new Channel(loop,timerfd));
    channelptr->setReadCallBack(std::bind(timeout, timerfd));
    channelptr->enableRead();
    loop->loop();
}
/**  Test Reactor  **/
void testTimerQueue(){
    // loggerTest();
    loop= new EventLoop;
    TimerQueue base(loop);
    Timestamp timestamp(Timestamp::now());

    std::thread t([](){
        loop->runAfter(3,std::bind(&timeout, 1));
    });


    loop->loop();
    t.join();
}

/**  Test RunInloop  **/


void testRunInLoop(){
    loop= new EventLoop;
    std::thread test([](){

        loop->runInLoop([](){
            std::cout << "run in loop 1" << std::endl;
            loop->runInLoop([](){
                std::cout << "run in loop loop" << std::endl;
            });

        });


    });
    std::thread t([](){
        loop->runAfter(3,std::bind(&timeout, 1));
    });

    loop->loop();
}
/**  Test EventLoopThread  **/
void runInThread()
{
    printf("runInThread(): pid = %d, tid = %d\n",
           getpid(), Jimmy::CurrentThread::tid());
}
void testEventLoopThread(){
    EventLoopThread t;
    EventLoop * loop_ =  t.startLoop();
    loop_->runInLoop(runInThread);
    usleep(1);
    loop_->runAfter(2, runInThread);
    sleep(3);
    loop_->quit();
}

/** Test Acceptor**/
void testAccepotr(){
    loop= new EventLoop;
    InetAddress addr(2333);
    Acceptor acceptor(addr,loop);
    acceptor.setNewConnectionCallback([](int fd,const InetAddress & addr){
        LOG_TRACE<<"new connection";
        Buffer inputBuffer;
        int savedErrno;
        inputBuffer.readFd(fd,&savedErrno);
        LOG_TRACE<<inputBuffer.retrieveAllAsString();
    });
    acceptor.listen();
    loop->loop();
}

/** Test Tcpserver**/
void testTcpserver(){
    loop= new EventLoop;
    InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,loop);
    tcpServer.setOnMessageCallback([](Buffer * buff,TcpServer::TcpConnectionptr conn){
        std::string msg =  buff->retrieveAllAsString();
        LOG_TRACE << "content:" << msg;
        std::thread t([conn,msg]() {
            sleep(3);
            LOG_TRACE <<"Tcp conn after read 0 " << conn.use_count();
            conn->send(msg);
        });
        t.detach();

    });
    tcpServer.setOnConnectionCallback([](TcpServer::TcpConnectionptr){
        LOG_TRACE<<"onConnection:" <<"new Connection!";

    });
    loop->loop();
}

/**test Buffer**/
void testBuffer(){
  Buffer buffer;
  int savedErrno;
  //input 6 chars
  buffer.readFd(STDIN_FILENO,&savedErrno);
  // test readfd
  std::cout << buffer.debugInfo() << std::endl;
  // test retrieveAllAsString
  std::string s =buffer.retrieveAsString(4);
  std::cout << "content:"<< s  << std::endl;
  std::cout << buffer.debugInfo() << std::endl;
    //input 4 chars for test  makeSpace
  buffer.readFd(STDIN_FILENO,&savedErrno);
  std::cout << buffer.debugInfo() << std::endl;
  std::cout << "content:"<< buffer.peekAllAsString()<< std::endl;
    //input 4  chars for test resize the buffer
    buffer.readFd(STDIN_FILENO,&savedErrno);
    std::cout << buffer.debugInfo() << std::endl;
    std::cout << "content:"<< buffer.peekAllAsString()<< std::endl;


};

int main() {
   // loggerTest();
    //testEventLoop();
    //testTimerQueue();
    //testRunInLoop();
   // testEventLoopThread();
    // testAccepotr();
    testTcpserver();
    //testBuffer();

}