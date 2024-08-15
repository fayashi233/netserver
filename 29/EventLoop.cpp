#include "EventLoop.h"

EventLoop::EventLoop():ep_(new Epoll)
{

}
EventLoop::~EventLoop()
{
    delete ep_;
}

// #include <unistd.h>
// #include <sys/syscall.h>

void EventLoop::run()
{
    //printf("EventLoop::run() thread is %d.\n",syscall(SYS_gettid));
    while (true)        // 事件循环。
    {
        std::vector<Channel*> channels = ep_->loop(10*1000);       //存放epoll_wait()返回的事件

        //如果channels为空，表示超时，回调TcpServer::epolltimeout()
        if(channels.size()==0)
        {
            epolltimeoutcallback_(this);
        }

        for(auto &ch : channels)
        {
            ch->handleevent();
        }
    }
}



void EventLoop::updatechannel(Channel *ch)        //把channel 添加/更新 到红黑树上
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel *ch)        //从红黑树上删除Channel
{
    ep_->removechannel(ch);
}


void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)   //设置epoll_wait()超时回调函数
{
    epolltimeoutcallback_ = fn;
}