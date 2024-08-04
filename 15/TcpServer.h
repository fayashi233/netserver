#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"

//TCP网络服务类
class TcpServer
{
private:
    EventLoop loop_;    //一个TcpServer可以有多个事件循环，现在是单线程，暂时只用一个事件循环
    Acceptor *acceptor_;//一对一

public:
    TcpServer(const std::string &ip,const uint16_t port);
    ~TcpServer();

    void start();   //运行事件循环
    void newconnection(Socket *clientsock);//处理新客户端连接请求
};