#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"

class Connection
{
private:
    EventLoop *loop_;       //Connection对应的事件循环，在构造函数中传入，不属于Acceptor的成员
    Socket *clientsock_;
    Channel *clientchannel_;

public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();

    int fd() const;             //返回fd_成员
    std::string ip() const;
    uint16_t port() const;

    void closecallback();       //TCP连接关闭时的回调函数，供Channel回调
    void errorcallback();       //TCP连接错误时的回调函数，供Channel回调

};