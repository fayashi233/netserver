#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port ,int subthreadnum,int workthreadnum) 
    : tcpserver_(ip, port,subthreadnum),threadpool_(workthreadnum,"WORKS")
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

//printf("EchoServer::HandleNewConnection() thread is %d.\n",syscall(SYS_gettid));

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

void EchoServer::HandleMessage(Connection *conn, std::string& message) // 处理客户端的请求报文，在TcpServer类中回调此函数
{
    // printf("EchoServer::HandleMessage() thread is %d.\n",syscall(SYS_gettid));

    // message = "reply" + message;
    // conn->send(message.data(),message.size());
    // 根据业务的需求，增加其他的代码

    // 把业务添加到线程池的任务队列中
    threadpool_.addtask(std::bind(&EchoServer::OnMessage,this,conn,message));

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

void EchoServer::OnMessage(Connection *conn,std::string& message)      //处理客户端的请求报文，用于添加给线程池
{
    message = "reply" + message;
    conn->send(message.data(),message.size());
}