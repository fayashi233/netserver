#include "EchoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t port, int subthreadnum, int workthreadnum)
    : tcpserver_(ip, port, subthreadnum), threadpool_(workthreadnum, "WORKS")
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

void EchoServer::Stop()
{
    //停止工作线程
    threadpool_.stop();
    printf("工作线程已停止\n");
    //停止IO线程
    tcpserver_.stop();
}


void EchoServer::HandleNewConnection(spConnection conn) // 处理新客户端连接请求，在TcpServer类中回调此函数
{

    //std::cout << "New Connection Come in." << std::endl;
    printf("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
    // printf("EchoServer::HandleNewConnection() thread is %d.\n",syscall(SYS_gettid));

    // 根据业务的需求，增加其他的代码
}

void EchoServer::HandleClose(spConnection conn) // 关闭客户端的连接，在TcpServer类中回调此函数
{
    printf("connection closed(fd=%d,ip=%s,port=%d).\n",conn->fd(),conn->ip().c_str(),conn->port());
    //std::cout << "EchoServer conn close." << std::endl;
    // 根据业务的需求，增加其他的代码
}

void EchoServer::HandleError(spConnection conn) // 客户端的连接错误，在TcpServer类中回调此函数
{
    //std::cout << "EchoServer conn error." << std::endl;
    // 根据业务的需求，增加其他的代码
}

void EchoServer::HandleMessage(spConnection conn, std::string &message) // 处理客户端的请求报文，在TcpServer类中回调此函数
{
    // printf("EchoServer::HandleMessage() thread is %d.\n",syscall(SYS_gettid));

    // 不使用工作线程，表示在IO线程中计算
    if (threadpool_.size() == 0)
    {
        OnMessage(conn, message);
    }
    else
    {
        // 把业务添加到线程池的任务队列中
        threadpool_.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
    }
}

void EchoServer::HandleSendComplete(spConnection conn) // 数据发送完成后的回调，在TcpServer类中回调此函数
{
    //std::cout << "Message send complete." << std::endl;
    // 根据业务的需求，增加其他的代码
}

// void EchoServer::HandleTimeOut(EventLoop *loop) // epoll_wait()超时时，在TcpServer类中回调此函数
// {
//     std::cout << "EchoServer timeout." << std::endl;
//     //根据业务的需求，增加其他的代码
// }

void EchoServer::OnMessage(spConnection conn, std::string &message) // 处理客户端的请求报文，用于添加给线程池
{
    message = "reply" + message; // 回显业务

    conn->send(message.data(), message.size());
}