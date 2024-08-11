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
    EventLoop loop_;                    //一个TcpServer可以有多个事件循环，现在是单线程，暂时只用一个事件循环
    Acceptor *acceptor_;                //一对一
    std::map<int,Connection*> conns_;   //一个TcpServer有多个Connection对象，存放在map容器中    

    std::function<void(Connection*)> newconnectioncb_;          // 回调EchoServer::HandleNewConnection()
    std::function<void(Connection*)> closeconnectioncb_;        // 回调EchoServer::HandleClose()
    std::function<void(Connection*)> errorconnectioncb_;         // 回调EchoServer::HandleError()
    std::function<void(Connection*,std::string &message)> onmessagecb_;        // 回调EchoServer::HandleMessage()
    std::function<void(Connection*)> sendcompletecb_;            // 回调EchoServer::HandleSendComplete()
    std::function<void(EventLoop*)>  timeoutcb_;                       // 回调EchoServer::HandleTimeOut()

public:
    TcpServer(const std::string &ip,const uint16_t port);
    ~TcpServer();

    void start();   //运行事件循环
    void newconnection(Socket *clientsock);//处理新客户端连接请求，在Acceptor类中回调此函数
    void closeconnection(Connection *conn); //关闭客户端的连接，在Connection类中回调此函数
    void errorconnection(Connection *conn); //客户端的连接错误，在Connection类中回调此函数
    void onmessage(Connection *conn,std::string message);  //处理客户端的请求报文，在Connection类中回调此函数
    void sendcomplete(Connection *conn);    //数据发送完成后的回调
    void epolltimeout(EventLoop *loop);     //epoll_wait()超时时，在EventLoop类中回调此函数

    //设置回调函数
    ////////////////////////////////////////////////////////////////////////////////////
    void setnewconnectioncb(std::function<void(Connection*)> fn);
    void setcloseconnectioncb(std::function<void(Connection*)> fn);
    void seterrorconnectioncb(std::function<void(Connection*)> fn);
    void setonmessagecb(std::function<void(Connection*,std::string &message)> fn);
    void setsendcompletecb(std::function<void(Connection*)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
    ////////////////////////////////////////////////////////////////////////////////////


};