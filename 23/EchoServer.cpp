#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port) : tcpserver_(ip, port)
{
    tcpserver_.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
    tcpserver_.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
    // tcpserver_.settimeoutcb(std::bind(&EchoServer::HandleTimeOut, this, std::placeholders::_1));
}

EchoServer::~EchoServer()
{
}

void EchoServer::Start() // 启动服务
{
    tcpserver_.start();
}

void EchoServer::HandleNewConnection(Connection *conn) // 处理新客户端连接请求，在TcpServer类中回调此函数
{
    std::cout << "New Connection Come in." << std::endl;
    //根据业务的需求，增加其他的代码
}

void EchoServer::HandleClose(Connection *conn) // 关闭客户端的连接，在TcpServer类中回调此函数
{
    std::cout << "EchoServer conn close." << std::endl;
    //根据业务的需求，增加其他的代码
}

void EchoServer::HandleError(Connection *conn) // 客户端的连接错误，在TcpServer类中回调此函数
{
    std::cout << "EchoServer conn error." << std::endl;
    //根据业务的需求，增加其他的代码
}

void EchoServer::HandleMessage(Connection *conn, std::string message) // 处理客户端的请求报文，在TcpServer类中回调此函数
{
    message = "reply" + message;

    //把message中的内容发送给客户端
    size_t len = message.size();
    std::string tmpbuf((char*)&len,4);
    tmpbuf.append(message);

    conn->send(tmpbuf.data(),tmpbuf.size());
    //根据业务的需求，增加其他的代码
}

void EchoServer::HandleSendComplete(Connection *conn) // 数据发送完成后的回调，在TcpServer类中回调此函数
{
    std::cout << "Message send complete." << std::endl;
    //根据业务的需求，增加其他的代码
}

// void EchoServer::HandleTimeOut(EventLoop *loop) // epoll_wait()超时时，在TcpServer类中回调此函数
// {
//     std::cout << "EchoServer timeout." << std::endl;
//     //根据业务的需求，增加其他的代码
// }