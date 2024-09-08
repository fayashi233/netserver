#pragma once
#include <functional>
#include <memory>
#include <atomic>
#include <unistd.h>
#include <sys/syscall.h>
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Timestamp.h"

class EventLoop;
class Connection;
class Channel;
using spConnection = std::shared_ptr<Connection>;

class Connection:public std::enable_shared_from_this<Connection>
{
private:
    EventLoop* loop_;       //Connection对应的事件循环，在构造函数中传入，不属于Acceptor的成员
    std::unique_ptr<Socket> clientsock_;
    std::unique_ptr<Channel> clientchannel_;

    Buffer inputbuffer_;    //接收缓冲区
    Buffer outputbuffer_;   //发送缓冲区
    std::atomic_bool disconnect_; //客户端连接是否已经断开，如果已断开，设置为true

    std::function<void(spConnection)> closecallback_;    //关闭fd_的回调函数，将回调TcpServer::closeconnection()
    std::function<void(spConnection)> errorcallback_;    //fd_发生错误的回调函数，将回调TcpServer::errorconnection()
    std::function<void(spConnection,std::string&)> onmessagecallback_;//处理报文的回调函数，将回调TcpServer::onmessage()
    std::function<void(spConnection)> sendcompletecallback_; //数据发生完成后的回调函数，将回调TcpServer::sendcomplete()

    Timestamp lastatime_;   //时间戳，创建Connection对象时为当前时间，每接收到一个报文，把时间戳更新为当前时间

public:
    Connection(EventLoop* loop,std::unique_ptr<Socket> clientsock);
    ~Connection();

    int fd() const;             //返回fd_成员
    std::string ip() const;
    uint16_t port() const;

    void onmessage();           //处理对端发过来的报文
    void closecallback();       //TCP连接关闭时的回调函数，供Channel回调
    void errorcallback();       //TCP连接错误时的回调函数，供Channel回调
    void writecallback();       //处理写事件的回调函数，供Channel回调

    void setclosecallback(std::function<void(spConnection)> fn);//设置关闭fd_的回调函数
    void seterrorcallback(std::function<void(spConnection)> fn);//设置fd_发生错误的回调函数
    void setonmessagecallback(std::function<void(spConnection,std::string&)> fn);//设置处理报文的回调函数
    void setsendcompletecallback(std::function<void(spConnection)> fn);//设置数据发生完成后的回调函数

    //发送数据，不管在任何线程中，都是调用此函数发送数据
    void send(const char *data,size_t size);    

    //发送数据，如果当前线程是IO线程，直接调用此函数，如果是工作线程，将此函数传给IO线程
    //void sendinloop(const char *data,size_t size);
    void sendinloop(std::shared_ptr<std::string> data);

    bool timeout(time_t now,int val);   //判断TCP连接是否超时

    
};