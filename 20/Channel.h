#pragma once
#include<sys/epoll.h>
//#include"Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include <functional>
#include "EventLoop.h"
class Epoll;
class EventLoop;

class Channel
{
private:
    int fd_ = -1;                           // Channel拥有的fd, Channel和fd是一对一关系
    // Epoll *ep_ = nullptr;                // Channel拥有的红黑树，一个Channel只对应一个Epoll
    EventLoop *loop_ = nullptr;             // Channel对应的事件循环，Channel与EventLoop是多对一的关系，一个Channel只对应一个EventLoop
    bool inepoll_ = false;                  // Channel是否已经添加到红黑树上如果未添加，调用epoll_ctl()的使用用EPOLL_CTL_ADD，否则用EPOLL_CTL_MOD
    uint32_t events_ = 0;                   // fd需要监视的事件。listenfd和clientfd需要监视EPOLLIN，clientfd还可能需要监视EPOLOUT
    uint32_t revents_ = 0;                  // fd已经发生的事件
    std::function<void()> readcallback_;    // fd读事件的回调函数
    std::function<void()> closecallback_;   // 关闭fd的回调函数，将回调Connection::closecallback()
    std::function<void()> errorcallback_;   // fd_发送错误的回调函数，将回调Connection::errorcallback()

public:
    Channel(EventLoop *loop,int fd);
    ~Channel();

    int fd();                    // 返回fd_成员
    void useet();                // 采用边缘触发
    void enablereading();        // 让epoll_wait()监视fd_的读事件
    void setinepoll();           // 把inepoll_成员的值设置为true
    void setrevent(uint32_t ev); // 设置revents_的值为ev
    bool inpoll();               // 返回inepoll成员
    uint32_t events();           // 返回events_成员
    uint32_t revents();          // 返回revents_成员
    void handleevent();          // 事件处理函数，epoll_wait()返回的时候，执行它
//    void onmessage();                   //处理对端发过来的报文

    void setreadcallback(std::function<void()> fn);//设置fd_读事件的回调函数
    void setclosecallback(std::function<void()> fn);//设置fd_关闭的回调函数
    void seterrorcallback(std::function<void()> fn);//设置fd_错误的回调函数
  
};

