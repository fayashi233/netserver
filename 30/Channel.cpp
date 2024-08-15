#include "Channel.h"
Channel::Channel(const std::unique_ptr<EventLoop>& loop, int fd ) : loop_(loop), fd_(fd)
{
}

Channel::~Channel()
{
}

int Channel::fd() // 返回fd_成员
{
    return fd_;
}

void Channel::useet() // 采用边缘触发
{
    events_ = events_ | EPOLLET;
}

void Channel::enablereading() // 让epoll_wait()监视fd_的读事件
{
    events_ = events_ | EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::disablereading()      //取消读事件
{
    events_ &=~EPOLLIN;
    loop_->updatechannel(this);
}

void Channel::enablewriting()       //注册写事件
{
    events_ |= EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disablewriting()      //取消写事件
{
    events_ &= ~EPOLLOUT;
    loop_->updatechannel(this);
}

void Channel::disableall()          //取消全部的事件
{
    events_ = 0;
    loop_->updatechannel(this);
}

void Channel::remove()              //从事件循环中删除Channel
{
    //disableall();
    loop_->removechannel(this); //从红黑树上删除fd
}

void Channel::setinepoll() // 把inepoll_成员的值设置为true
{
    inepoll_ = true;
}

void Channel::setrevent(uint32_t ev) // 设置revents_的值为ev
{
    revents_ = ev;
}

bool Channel::inpoll() // 返回inepoll成员
{
    return inepoll_;
}

uint32_t Channel::events() // 返回events_成员
{
    return events_;
}

uint32_t Channel::revents() // 返回revents_成员
{
    return revents_;
}

void Channel::handleevent() // 事件处理函数，epoll_wait()返回的时候，执行它
{
    ////////////////////////////////////////////////////////////////////////
    if (revents_ & EPOLLRDHUP)                // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    {
        //printf("EPOLLRDHUP\n");
        //remove();       //从事件循环中删除Channel
        closecallback_();

    }                                //  普通数据  带外数据
    else if (revents_ & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
    {
        //printf("EPOLLIN|EPOLLPRI\n");
        readcallback_();
    }
    else if (revents_ & EPOLLOUT)             // 有数据需要写，暂时没有代码，以后再说。
    {
        //printf("EPOLLOUT\n");
        writecallback_();
    }
    else                                            // 其它事件，都视为错误。
    {
        //printf("others\n");
        //remove();
        errorcallback_();
    }
}


void Channel::setreadcallback(std::function<void()> fn)//设置fd_读事件的回调函数
{
    readcallback_ = fn;
}

void Channel::setclosecallback(std::function<void()> fn)//设置fd_关闭的回调函数
{
    closecallback_ = fn;
}

void Channel::seterrorcallback(std::function<void()> fn)//设置fd_错误的回调函数
{
    errorcallback_ = fn;
}

void Channel::setwritecallback(std::function<void()> fn)//设置fd_错误的回调函数
{
    writecallback_ = fn;
}