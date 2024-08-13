#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"

/*
    EchoServer类：回显服务器
*/

class EchoServer
{
private:
    TcpServer tcpserver_;
public:
    EchoServer(const std::string &ip,const uint16_t port);
    ~EchoServer();

    void Start();       //启动服务

    void HandleNewConnection(Connection *conn);//处理新客户端连接请求，在TcpServer类中回调此函数
    void HandleClose(Connection *conn); //关闭客户端的连接，在TcpServer类中回调此函数
    void HandleError(Connection *conn); //客户端的连接错误，在TcpServer类中回调此函数
    void HandleMessage(Connection *conn,std::string& message);  //处理客户端的请求报文，在TcpServer类中回调此函数
    void HandleSendComplete(Connection *conn);    //数据发送完成后的回调，在TcpServer类中回调此函数
    
    //不关心的事件可以不写
    //void HandleTimeOut(EventLoop *loop);     //epoll_wait()超时时，在TcpServer类中回调此函数
};

