#pragma once
#include "Epoll.h"

//事件循环类
class EventLoop
{
private:
    Epoll *ep_;
public:
    EventLoop();    //在构造函数中创建epoll对象
    ~EventLoop();   //在析构函数中销毁ep_

    void run();     //运行事件循环
    Epoll *ep();    //返回ep_

};