#include "Epoll.h"

Epoll::Epoll()
{
    if((epollfd_ = epoll_create(1)) == -1)
    {
        printf("epoll_create() failed(%d).\n",errno); exit(-1);
    }
}

Epoll::~Epoll()
{
    close(epollfd_);
}

// void Epoll::addfd(int fd, uint32_t op)
// {
//     epoll_event ev;              // 声明事件的数据结构。
//     ev.data.fd=fd;                 // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
//     ev.events=op;                // 让epoll监视listenfd的读事件，采用水平触发。
//     if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&ev) == -1)
//     {
//         printf("epoll_ctl() failed.\n",errno);exit(-1);
//     }
// }

void Epoll::updatechannel(Channel *ch)
{
    epoll_event ev;
    ev.data.ptr = ch;
    ev.events = ch->events();//指定事件

    //判断Channel是否已经在树上
    if(ch->inpoll())
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_MOD,ch->fd(),&ev) == -1)
        {
            perror("epoll_ctl() failed.\n");exit(-1);
        }
    }
    else
    {
        if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,ch->fd(),&ev) == -1)
        {
            perror("epoll_ctl() failed.\n");exit(-1);
        }
        ch->setinepoll();
    }
}



// std::vector<epoll_event> Epoll::loop(int timeout)
// {
//    std::vector<epoll_event> evs;    //存放epoll_wait()返回的事件
   
//    bzero(events_,sizeof(events_));
//    int infds=epoll_wait(epollfd_,events_,MaxEvents,timeout);       // 等待监视的fd有事件发生。

//     // 返回失败。
//     if (infds < 0)
//     {
//         perror("epoll_wait() failed"); exit(-1);
//     }

//     // 超时。
//     if (infds == 0)
//     {
//         printf("epoll_wait() timeout.\n"); return evs;
//     }

//     // 如果infds>0，表示有事件发生的fd的数量。
//     for (int ii=0;ii<infds;ii++)       // 遍历epoll返回的数组evs。
//     {
//         evs.push_back(events_[ii]);
//     }

//     return evs;
// }


std::vector<Channel*> Epoll::loop(int timeout)
{
   std::vector<Channel*> channels;    //存放epoll_wait()返回的事件
   
   bzero(events_,sizeof(events_));
   int infds=epoll_wait(epollfd_,events_,MaxEvents,timeout);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        perror("epoll_wait() failed"); exit(-1);
    }

    // 超时。
    if (infds == 0)
    {
        printf("epoll_wait() timeout.\n"); return channels;
    }

    // 如果infds>0，表示有事件发生的fd的数量。
    for (int ii=0;ii<infds;ii++)       // 遍历epoll返回的数组evs。
    {
        //evs.push_back(events_[ii]);
        Channel *ch = (Channel*)events_[ii].data.ptr;
        ch->setrevent(events_[ii].events);
        channels.push_back(ch);

    }

    return channels;
}
