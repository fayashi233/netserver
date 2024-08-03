#include "Channel.h"
Channel::Channel(Epoll *ep,int fd):ep_(ep),fd_(fd)
{
}

Channel::~Channel()
{
}
// class Channel
// {
// private:
//     int fd_=-1;             //Channel拥有的fd, Channel和fd是一对一关系
//     Epoll *ep_ = nullptr;   //Channel拥有的红黑树，一个Channel只对应一个Epoll
//     bool inepoll_=false;    //Channel是否已经添加到红黑树上如果未添加，调用epoll_ctl()的使用用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
//     uint32_t events_=0;     //fd需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLOUT
//     uint32_t revents_=0;   //fd已经发生的事件
    
// public:
//     Channel(Epoll *ep,int fd);
//     ~Channel();

//     int fd();                    // 返回fd_成员
//     void useet();                // 采用边缘触发
//     void enablereading();        // 让epoll_wait()监视fd_的读事件
//     void setinepoll();           // 把inepoll_成员的值设置为true
//     void setrevent(uint32_t ev); // 设置revents_的值为ev
//     bool inpoll();               // 返回inepoll成员
//     uint32_t events();           // 返回events_成员
//     uint32_t revents();          // 返回revents_成员
// };
int Channel::fd()                    // 返回fd_成员
{
    return fd_;
}

void Channel::useet()                // 采用边缘触发
{
    events_ = events_|EPOLLET;
}

void Channel::enablereading()        // 让epoll_wait()监视fd_的读事件
{
    events_ = events_|EPOLLIN;
    ep_->updatechannel(this);
}

void Channel::setinepoll()           // 把inepoll_成员的值设置为true
{
    inepoll_ = true;
}

void Channel::setrevent(uint32_t ev) // 设置revents_的值为ev
{
    revents_ = ev;
}

bool Channel::inpoll()               // 返回inepoll成员
{
    return inepoll_;
}

uint32_t Channel::events()           // 返回events_成员
{
    return events_;
}

uint32_t Channel::revents()          // 返回revents_成员
{
    return revents_;
}




