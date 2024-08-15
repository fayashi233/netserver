#pragma once
#include <functional>
#include <memory>
#include <atomic>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"

class Connection;
using spConnection = std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop *loop_;       //Connection对应的事件循环，在构造函数中传入，不属于Acceptor的成员
    Socket *clientsock_;
    Channel *clientchannel_;

    Buffer inputbuffer_;    //接收缓冲区
    Buffer outputbuffer_;   //发送缓冲区
    std::atomic_bool disconnect_; //客户端连接是否已经断开，如果已断开，设置为true

    std::function<void(spConnection)> closecallback_;    //关闭fd_的回调函数，将回调TcpServer::closeconnection()
    std::function<void(spConnection)> errorcallback_;    //fd_发生错误的回调函数，将回调TcpServer::errorconnection()
    std::function<void(spConnection,std::string&)> onmessagecallback_;//处理报文的回调函数，将回调TcpServer::onmessage()
    std::function<void(spConnection)> sendcompletecallback_; //数据发生完成后的回调函数，将回调TcpServer::sendcomplete()

public:
    Connection(EventLoop *loop,Socket *clientsock);
    ~Connection();

    int fd() const;             //返回fd_成员
    std::string ip() const;
    uint16_t port() const;

    void closecallback();       //TCP连接关闭时的回调函数，供Channel回调
    void errorcallback();       //TCP连接错误时的回调函数，供Channel回调
    void writecallback();       //处理写事件的回调函数，供Channel回调

    void setclosecallback(std::function<void(spConnection)> fn);//设置关闭fd_的回调函数
    void seterrorcallback(std::function<void(spConnection)> fn);//设置fd_发生错误的回调函数
    void setonmessagecallback(std::function<void(spConnection,std::string&)> fn);//设置处理报文的回调函数
    void setsendcompletecallback(std::function<void(spConnection)> fn);//设置数据发生完成后的回调函数

    void send(const char *data,size_t size);        //发送数据


    void onmessage();                   //处理对端发过来的报文
};