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


void Epoll::removechannel(Channel *ch)      //从红黑树上删除channel
{
    //判断Channel是否已经在树上
    if(ch->inpoll())
    {
        //printf("removechannel()\n");
        if(epoll_ctl(epollfd_,EPOLL_CTL_DEL,ch->fd(),0) == -1)
        {
            perror("epoll_ctl() failed.\n");exit(-1);
        }
    }
}

std::vector<Channel*> Epoll::loop(int timeout)
{
   std::vector<Channel*> channels;    //存放epoll_wait()返回的事件
   
   bzero(events_,sizeof(events_));
   int infds=epoll_wait(epollfd_,events_,MaxEvents,timeout);       // 等待监视的fd有事件发生。

    // 返回失败。
    if (infds < 0)
    {
        // EBADF: epfd不是一个有效的描述符
        // EFAULT: 参数events指向的内存区域不可写
        // EINVAL: epfd不是一个epoll文件描述符，或者maxevents小于等于0
        // EINTR: 阻塞过程中被信号中断，epoll_wait()可以避免，或者错误处理中，解析error后重新调用epoll_wait()
        perror("epoll_wait() failed"); exit(-1);
    }

    // 超时。
    if (infds == 0)
    {
        //printf("epoll_wait() timeout.\n"); 
        return channels;
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
