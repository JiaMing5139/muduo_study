#include <iostream>
#include "log/logger.h"
#include "EventLoop.h"
#include <assert.h>
#include <thread>
#include "Epoll.h"
#include <sys/socket.h>
#include <memory>

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
#include "httpForTest/httpRequest.h"
#include "httpForTest/Response.h"
#include "LoopThreadPool.h"
#include "TcpClient.h"
#include "Timer/TimeWheel.h"

/**  Test Asynlog  **/

std::unique_ptr<AsyncLogging> asyn(new AsyncLogging("fuck"));
void g_output(const char * msg,size_t len){
    asyn->append(msg,len);
}

void setAsynLog(){
  //  Jimmy::Logger::setOutput(g_output);
    Jimmy::Logger::setLevel(Jimmy::Logger::TRACE);
    asyn->start();
}

void loggerTest() {
    setAsynLog();

    int i = 0;
    while(i < 100){
        LOG_TRACE << "testLogaa";
        i++;
    }

}


/**  Test Reactor  **/
EventLoop* loop ;

void timeout(int fd){
    printf("timeout\n");
}
#ifdef __linux__
#include <sys/timerfd.h>
void testEventLoop(){
    loop= new EventLoop;
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 2;
    timerfd_settime(timerfd,0,&howlong,nullptr);
    std::shared_ptr<Channel> channelptr (new Channel(loop,timerfd));
    channelptr->setReadCallBack(std::bind(timeout, timerfd));
    channelptr->enableRead();
    loop->loop();
}
#endif
/**  Test Reactor  **/
void testTimerQueue(){
    // loggerTest();
    loop= new EventLoop;
    TimerQueue base(loop);
    Timestamp timestamp(Timestamp::now());
    TimerId id;
    std::thread t([&](){
      id =  loop->runEvery(2,std::bind(&timeout, 1));
        loop->cancleTimer(id);
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


}

/** Test downLoadServer 测试大文件发送**/
std::string readFile(const char* filename)
{
    std::string content;
    FILE* fp = ::fopen(filename, "rb");
    if (fp)
    {
        const int kBufSize = 1024*1024;
        char iobuf[kBufSize];
        ::setbuffer(fp, iobuf, sizeof iobuf);

        char buf[kBufSize];
        size_t nread = 0;
        while ( (nread = ::fread(buf, 1, sizeof buf, fp)) > 0)
        {
            content.append(buf, nread);
        }
        ::fclose(fp);
    }
    LOG_TRACE <<content.length();
    return content;
}
void downloadTest(TcpServer::TcpConnectionptr conn){
    std::string msg = readFile("testfile");
    conn->send(msg);
    conn->shutdown();
}
void testDownloadserver(){
    loop= new EventLoop;
    InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,loop);
    tcpServer.setOnMessageCallback([](Buffer * buff,TcpServer::TcpConnectionptr conn){
        std::string msg =  buff->retrieveAllAsString();
        LOG_TRACE << "content:" << msg;
        std::thread t([conn,msg]() {
            //sleep(3);
            LOG_TRACE <<"Tcp conn after read 0 " << conn.use_count();
            conn->send(msg);
        });
        t.detach();

    });
    tcpServer.setOnConnectionCallback(downloadTest);
    loop->loop();
}
/**test upload Server 测试大文件接收 **/
void testUploadServer(){
    loop= new EventLoop;
    InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,loop);
    tcpServer.start();
    tcpServer.setOnMessageCallback([](Buffer * buff,TcpServer::TcpConnectionptr conn){
        FILE * file = fopen("uploadFile","wb");
        if(!file){
            LOG_SYSFATAL<<"fopen";
        }
        string msg = buff->retrieveAllAsString();
        int nread = fwrite(msg.c_str(),1,msg.length(),file);

    });
    tcpServer.setOnConnectionCallback(downloadTest);
    loop->loop();
}

/** test echo server**/

void testEchoserver(){
   loop= new EventLoop;

    InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,loop);
    tcpServer.start();
    tcpServer.setOnMessageCallback([](Buffer * buf,TcpServer::TcpConnectionptr conn){
        conn->send(buf->retrieveAllAsString());
    });
    tcpServer.setOnConnectionCallback([](TcpServer::TcpConnectionptr conn){

    });
   loop->loop();
}

/**single thread Httpserver**/

void testHttpServer(){

    loop= new EventLoop;
    InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,loop);
    tcpServer.start();
    tcpServer.setOnMessageCallback([](Buffer * buf,TcpServer::TcpConnectionptr conn){
        httpRequest request ;
        string msg(buf->retrieveAllAsString());
        request.parseContext(msg);

        if(request.ifFinish()){
                Response r;
                string res =  "test";
                r.setStatusCode(Response::k200Ok);
                r.setBody(res);
                Buffer buf;
                r.appendToBuff(&buf);
                conn->send(&buf);
                conn->shutdown();
            }

    });
    tcpServer.setOnConnectionCallback([](TcpServer::TcpConnectionptr conn){

    });
    loop->loop();
}

/**test EventThreaddPool**/

void testEventThreaddPool() {
     loop= new EventLoop;
     LoopThreadPool pool(loop,1);
     pool.start();
     EventLoop * evpool = pool.getLoop();
     InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,evpool);
    tcpServer.setOnMessageCallback([](Buffer * buf,TcpServer::TcpConnectionptr conn){
        conn->send(buf->retrieveAllAsString());
    });
    tcpServer.setOnConnectionCallback([](TcpServer::TcpConnectionptr conn){

    });
    loop->loop();

}

/**test block_testTcpClient**/

void testTcpClient(){
    loop= new EventLoop;
    InetAddress addr("10.211.55.2",23333);
    LOG_TRACE << "connect:" << addr ;
    TcpClient tcpClient(loop,addr);
    tcpClient.setOnMessageCallback([](Buffer * buf,TcpServer::TcpConnectionptr conn){
        LOG_TRACE << "onMessage:" << conn->peerAddr() ;
        conn->send(buf->retrieveAllAsString());
    });
    tcpClient.setOnConnectionCallback([](TcpServer::TcpConnectionptr conn){
        LOG_TRACE << "connect successful" ;
    });
    tcpClient.start();
    loop->loop();
}

/**test TimeWheel**/
void testTimeWheel(){
    loop= new EventLoop;

    InetAddress addr(2333);
    std::cout << addr << std::endl;
    TcpServer tcpServer(addr,loop);
    tcpServer.start();
    TimeWheel timeWheel(loop,5); // kick out connection without talking for 3second
    tcpServer.setOnMessageCallback([&](Buffer * buf,TcpServer::TcpConnectionptr conn){
        auto  k = conn->getTimeWheelEntry();
        timeWheel.upadateConnection(k);
        conn->send(buf->retrieveAllAsString());
    });
    tcpServer.setOnConnectionCallback([&](TcpServer::TcpConnectionptr conn){
        auto enty =  timeWheel.addConnection(conn);
        conn->setTimeWheelEntry(enty);
    });
    loop->loop();
}



int main() {
     setAsynLog();
    //loggerTest();
    //testEventLoop();
   // testTimerQueue();
    //testRunInLoop();
   // testEventLoopThread();
    // testAccepotr();
     //testDownloadserver();
    //testBuffer();
     // testHttpServer();
  //  testEchoserver();
    // testEventThreaddPool();
    //testConnector();
   // testTcpClient();
   //testChatServer();
    testTimeWheel();


}