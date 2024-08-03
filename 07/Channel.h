#pragma once
#include<sys/epoll.h>
#include"Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
class Epoll;


class Channel
{
private:
    int fd_=-1;             //Channel拥有的fd, Channel和fd是一对一关系
    Epoll *ep_ = nullptr;   //Channel拥有的红黑树，一个Channel只对应一个Epoll
    bool inepoll_=false;    //Channel是否已经添加到红黑树上如果未添加，调用epoll_ctl()的使用用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
    uint32_t events_=0;     //fd需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLOUT
    uint32_t revents_=0;    //fd已经发生的事件
    bool islisten_=false;   //listenfd取值为true，客户端连上来的fd取值为false 

public:
    Channel(Epoll *ep,int fd,bool islisten);
    ~Channel();

    int fd();                    // 返回fd_成员
    void useet();                // 采用边缘触发
    void enablereading();        // 让epoll_wait()监视fd_的读事件
    void setinepoll();           // 把inepoll_成员的值设置为true
    void setrevent(uint32_t ev); // 设置revents_的值为ev
    bool inpoll();               // 返回inepoll成员
    uint32_t events();           // 返回events_成员
    uint32_t revents();          // 返回revents_成员
    void handleevent(Socket *servsock);          // 事件处理函数，epoll_wait()返回的时候，执行它
};

