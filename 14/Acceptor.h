#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor
{
private:
    EventLoop *loop_;       //Acceptor对应的事件循环，在构造函数中传入，不属于Acceptor的成员
    Socket *servsock_;
    Channel *acceptchannel_;

public:
    Acceptor(EventLoop *loop,const std::string &ip, const uint16_t port);
    ~Acceptor();
    void newconnection();//处理新客户端连接请求
};