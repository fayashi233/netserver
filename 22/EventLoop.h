#pragma once
#include <functional>
#include "Epoll.h"

class Channel;
class Epoll;


//事件循环类
class EventLoop
{
private:
    Epoll *ep_;
    std::function<void(EventLoop*)> epolltimeoutcallback_;  //epoll_wait()超时回调函数
public:
    EventLoop();    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁ep_

    void run();     //运行事件循环
    //Epoll *ep();    //返回ep_
    void updatechannel(Channel *ch);        //把channel 添加/更新 到红黑树上_
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);   //设置epoll_wait()超时回调函数
};