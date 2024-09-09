#pragma once
#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <map>
#include <atomic>
#include "Connection.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>   //定时器的头文件
#include "Epoll.h"

class Channel;
class Epoll;
class Connection;

using spConnection = std::shared_ptr<Connection>;

//事件循环类
class EventLoop
{
private:
    int timetvl_;                       //闹钟的时间间隔，单位：秒
    int timeout_;                       //Connection对象超时的时间，单位：秒
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;  //epoll_wait()超时回调函数
    pid_t threadid_;    //时间循环所在线程的id
    std::queue<std::function<void()>> taskqueue_;   //事件循环被eventfd唤醒后执行的任务队列
    std::mutex mutex_;      //队列的互斥锁
    
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_;  //eventfd的Channel
    int timerfd_;   //定时器的fd
    std::unique_ptr<Channel> timerchannel_; //定时器的Channel
    bool mainloop_;         //true是主事件循环，false是从事件循环
    std::map<int,spConnection> conns_;
    std::mutex mmutex_;                                                    //保护conns_的互斥锁

    std::function<void(int)> timercallback_;    //删除TcpServer中超时的Connection对象，将被设置为TcpServer::removeconn()
    std::atomic_bool stop_;                     //初始值为false，如果设置为true，表示停止事件循环


public:
    EventLoop(int mainloop,int timetvl=30,int timeout=80);    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁ep_

    void run();     //运行事件循环
    void stop();    //停止事件循环
    //Epoll *ep();    //返回ep_
    void updatechannel(Channel *ch);        //把channel 添加/更新 到红黑树上_
    void removechannel(Channel *ch);        //从红黑树上删除Channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);   //设置epoll_wait()超时回调函数

    bool isinloopthread();      //判断当前线程是否为事件循环线程

    void queueinloop(std::function<void()> fn);  //将任务添加到队列中
    void wakeup();      //唤醒事件循环
    void handlewakeup();    //事件循环被eventfd唤醒后执行的函数

    void handletimer();     // 闹钟响时执行的函数
    
    void newconnection(spConnection conn);  //把Connection对象保存在conns_中
    void settimercallback(std::function<void(int)> fn);

};