#pragma once
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include <map>
//TCP网络服务类
class TcpServer
{
private:
    EventLoop loop_;    //一个TcpServer可以有多个事件循环，现在是单线程，暂时只用一个事件循环
    Acceptor *acceptor_;//一对一
    std::map<int,Connection*> conns_;   //一个TcpServer有多个Connection对象，存放在map容器中    
public:
    TcpServer(const std::string &ip,const uint16_t port);
    ~TcpServer();

    void start();   //运行事件循环
    void newconnection(Socket *clientsock);//处理新客户端连接请求，在Acceptor类中回调此函数

    void closeconnection(Connection *conn); //关闭客户端的连接，在Connection类中回调此函数
    void errorconnection(Connection *conn); //客户端的连接错误，在Connection类中回调此函数

};