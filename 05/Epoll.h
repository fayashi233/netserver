#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>

//Epoll类
class Epoll
{
private:
    static const int MaxEvents = 100;
    int epollfd_ = -1; //epoll句柄
    epoll_event events_[MaxEvents];
public:
    Epoll();
    ~Epoll();
    void addfd(int fd, uint32_t op);    //把fd和它需要监视的时间添加到红黑树上
    std::vector<epoll_event> loop(int timeout = -1);    //运行epoll_wait()，等待已发生的事件，用容器vector返回
};