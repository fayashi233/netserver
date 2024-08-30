#pragma once
#include <functional>
#include <memory>
#include <queue>
#include <mutex>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/timerfd.h>   //定时器的头文件
#include "Epoll.h"

class Channel;
class Epoll;


//事件循环类
class EventLoop
{
private:
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

public:
    EventLoop(int mainloop);    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁ep_

    void run();     //运行事件循环
    //Epoll *ep();    //返回ep_
    void updatechannel(Channel *ch);        //把channel 添加/更新 到红黑树上_
    void removechannel(Channel *ch);        //从红黑树上删除Channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);   //设置epoll_wait()超时回调函数

    bool isinloopthread();      //判断当前线程是否为事件循环线程

    void queueinloop(std::function<void()> fn);  //将任务添加到队列中
    void wakeup();      //唤醒事件循环
    void handlewakeup();    //事件循环被eventfd唤醒后执行的函数

    void handletimer();     // 闹钟响时执行的函数

};