#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port)
{
    acceptor_ = new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
    loop_.setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
}

TcpServer::~TcpServer()
{
    delete acceptor_;
    for(auto &aa:conns_)
    {
        delete aa.second;//释放全部Connection对象
    } 
}

void TcpServer::start()
{
    loop_.run();
}

void TcpServer::newconnection(Socket *clientsock)//处理新客户端连接请求
{
    Connection *conn = new Connection(&loop_,clientsock); //这里new出来的没有释放，以后再解决
    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));

    conns_[conn->fd()] = conn;
    //printf ("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());


    if(newconnectioncb_) newconnectioncb_(conn);         //回调EchoServer::HandleNewConnection()
}

void TcpServer::closeconnection(Connection *conn)//关闭客户端的连接，在Connection类中回调此函数
{
    //先回调EchoServer::HandleClose()，再关闭连接
    if(closeconnectioncb_)  closeconnectioncb_(conn);
    
    //printf("client(eventfd=%d) disconnected.\n",conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}

void TcpServer::errorconnection(Connection *conn)//客户端的连接错误，在Connection类中回调此函数
{
    //回调EchoServer::HandleError()
    if(errorconnectioncb_) errorconnectioncb_(conn);

    //printf("client(eventfd=%d) error.\n",conn->fd());
    conns_.erase(conn->fd());
    delete conn;
}


void TcpServer::onmessage(Connection *conn,std::string& message)  //处理客户端的请求报文，在Connection类中回调此函数
{
    // message = "reply" + message;

    // //把message中的内容发送给客户端
    // size_t len = message.size();
    // std::string tmpbuf((char*)&len,4);
    // tmpbuf.append(message);

    // conn->send(tmpbuf.data(),tmpbuf.size());

    if(onmessagecb_) onmessagecb_(conn,message); // 回调EchoServer::HandleMessage()

}

void TcpServer::sendcomplete(Connection *conn)    //数据发送完成后的回调
{
    //printf("send complete.\n");

    if(sendcompletecb_) sendcompletecb_(conn);  // 回调EchoServer::HandleSendComplete()
}

void TcpServer::epolltimeout(EventLoop *loop)     //epoll_wait()超时时，在EventLoop类中回调此函数
{
    //printf("epoll_wait() timeout.\n");

    if(timeoutcb_)  timeoutcb_(loop);  // 回调EchoServer::HandleTimeOut()
}

//设置回调函数
///////////////////////////////////////////////////////////////////////////////////////////////
void TcpServer::setnewconnectioncb(std::function<void(Connection*)> fn)
{
    newconnectioncb_=fn;
}

void TcpServer::setcloseconnectioncb(std::function<void(Connection*)> fn)
{
    closeconnectioncb_=fn;
}

void TcpServer::seterrorconnectioncb(std::function<void(Connection*)> fn)
{
    errorconnectioncb_=fn;
}

void TcpServer::setonmessagecb(std::function<void(Connection*,std::string &message)> fn)
{
    onmessagecb_=fn;
}

void TcpServer::setsendcompletecb(std::function<void(Connection*)> fn)
{
    sendcompletecb_=fn;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    timeoutcb_=fn;
}
////////////////////////////////////////////////////////////////////////////////////////////////