#pragma once
#include <functional>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connection
{
private:
    EventLoop *loop_;       //Connection对应的事件循环，在构造函数中传入，不属于Acceptor的成员
    Socket *clientsock_;
    Channel *clientchannel_;

    Buffer inputbuffer_;    //接收缓冲区
    Buffer outputbuffer_;   //发送缓冲区

    std::function<void(Connection*)> closecallback_;    //关闭fd_的回调函数，将回调TcpServer::closeconnection()
    std::function<void(Connection*)> errorcallback_;    //fd_发生错误的回调函数，将回调TcpServer::errorconnection()

public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();

    int fd() const;             //返回fd_成员
    std::string ip() const;
    uint16_t port() const;

    void closecallback();       //TCP连接关闭时的回调函数，供Channel回调
    void errorcallback();       //TCP连接错误时的回调函数，供Channel回调

    void setclosecallback(std::function<void(Connection*)> fn);//设置关闭fd_的回调函数
    void seterrorcallback(std::function<void(Connection*)> fn);//设置fd_发生错误的回调函数

    void onmessage();                   //处理对端发过来的报文
};