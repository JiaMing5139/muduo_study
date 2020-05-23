# muduo_study
从零用c++11实现muduo的核心功能


#### 参考书籍
```
Linux 多线程服务端编程
```

#### 编译
```
(Cmake Version >= 3.1.5)
Cmake CMakeLists.text

make
```
## 笔记
#### TcpConnection 

###### Q1:TcpConnection的状态设计（非阻塞）

event\state | closed |  closing | connected 
-|-|-|-
new connection | setState(Connected) | ❌ | ❌|
readable event|  ❌ | 1.读取数据到inputbuffer <br>2.调用onMessagecallback |1.读取数据到inputbuffer  <br>2.调用onMessagecallback|
send          | ❌ | ❌ |if(outputbuffer有数据){<br>添加数据到outputbuffer<br>} else{<br>尝试发送数据，若没有发送完，则添加到outputbufer中，并监听可写事件，若发送完毕调用writeCompleteCallback<br>}|
writeable event| ❌ | if(outputbuffer有数据) {<br>尝试发送outputbuffer中的数据，未发送的添加到outputbuffer中，若发送完毕，关闭监听可读事件,调用writeCompleteCallback，<br>继续shutdown<br>} |if(outputbuffer有数据){<br>尝试发送outputbuffer中的数据，未发送的添加到outputbuffer中，若发送完毕，关闭监听可读事件,调用writeCompleteCallback<br>}|
shutdown | ❌ | ❌ |if(outputbuffer有数据) {<br>setState(closing)<br>}else{调用shutdown}|
closed event(read == 0) | ❌ |if(outputbuffer有数据) {<br>丢弃数据<br>}<br>  在poll,和Tcpserver中析构资源<br>setState(closed) |if(outputbuffer有数据) {<br>丢弃数据<br>}<br> 在poll,和Tcpserver中析构资源<br>setState(closed) |


在关闭时，如果网络库管理的输出缓冲区还有数据，是否需要继续发送出去？如果遇到sigpipe则忽略掉？
###### Q2:TcpConnection的生命周期

###### Q3:TcpConnection相关的临界区和race condition

#### 定时器队列

###### Q1:为什么已经有了Timerfd，还要用TimerQueue去维护定时器的触发？
```
减少文件描述符的使用，若不用timerQueue，一秒内创建1000个定时器，则需要向epoll/poll注册1000个可读事件
而使用TimerQueue时，只维护一个最早的触发的fd,当TimerQueue中的timerfd事件触发时，触发后将一秒内的定时
器事件全部手动触发。这在每一秒内只用维护一个fd，一分钟最多60个。

相关实现:
Timer/TimerStamp:
    friend bool operator < (const Timestamp & ,const Timestamp &);
    friend bool operator == (const Timestamp & ,const Timestamp &);
    
Timer/TimerQueueBase
```     
###### Q2:TimerQueue中维护Timer的数据结构？
```
数据结构要能高效的找出最早触发的一批定时器。而且需要能快速的移除和添加定时器。
线性表：查找时间复杂度是O(N)
优先队列：一般用堆实现，查找时间复杂度O(LogN），因为二叉堆实现一般是用连续的内存是实现，
所以可以较好的利用局部性原理的缓存，效率高
二叉搜索树：红黑树实现，查找时间复杂度O(LogN) ，但二叉树一般是不连续空间，无法利用局部性原理，
效率比优先队列低。

具体实现：
数据结构（红黑树）: Timer/TimerQueue: set<pair<Timerstamp,shared_ptr<Timer>>>
定时器查找：Timer/TimerQueueBase : handleEvent
定时器添加: Timer/TimerQueueBase : addTimer

```
###### Q3：Timer的线程安全性，Timer属于EventLoop，而EventLoop中的成员会被多个线程看到，如果在多个线程向TimerQueue添加如何保证安全？

###### Q4: 如何实现对指定Timer的删除？

```

```


#### 异步日志


# To do list
- Threadpoll of EventLoop/one loop per thread
- 流量控制
- connector

