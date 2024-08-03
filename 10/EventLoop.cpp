#include "EventLoop.h"

EventLoop::EventLoop():ep_(new Epoll)
{

}
EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
    while (true)        // 事件循环。
    {
        std::vector<Channel*> channels = ep_->loop();       //存放epoll_wait()返回的事件
        for(auto &ch : channels)
        {
            ch->handleevent();
        }
    }
}

Epoll *EventLoop::ep()    //返回ep_
{
    return ep_;
}

void EventLoop::updatechannel(Channel *ch)        //把channel 添加/更新 到红黑树上
{
    ep_->updatechannel(ch);
}