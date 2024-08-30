#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port,int threadnum)
        :threadnum_(threadnum),mainloop_(new EventLoop(true)),acceptor_(mainloop_.get(),ip,port),threadpool_(threadnum_,"IO")
{
    //mainloop_(new EventLoop);  //主事件循环
    mainloop_->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));

    //acceptor_ = new Acceptor(mainloop_,ip,port);
    acceptor_.setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));

    //threadpool_ = new ThreadPool(threadnum_,"IO");   //创建线程池
    //创建从事件循环
    for(int ii=0;ii<threadnum_;ii++)
    {
        subloops_.emplace_back(new EventLoop(false));
        subloops_[ii]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout,this,std::placeholders::_1));
        threadpool_.addtask(std::bind(&EventLoop::run,subloops_[ii].get())); //在线程池中运行从事件循环
    }   

}

TcpServer::~TcpServer()
{
    //delete acceptor_;
    //delete mainloop_;
    // for(auto &aa:conns_)
    // {
    //     delete aa.second;//释放全部Connection对象
    // } 
    //delete threadpool_;
}

void TcpServer::start()
{
    mainloop_->run();
}

void TcpServer::newconnection(std::unique_ptr<Socket> clientsock)//处理新客户端连接请求
{
    //Connection *conn = new Connection(mainloop_,clientsock); //这里new出来的没有释放，以后再解决

    spConnection conn = spConnection(new Connection(subloops_[clientsock->fd()%threadnum_].get(),std::move(clientsock)));

    conn->setclosecallback(std::bind(&TcpServer::closeconnection,this,std::placeholders::_1));
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection,this,std::placeholders::_1));
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage,this,std::placeholders::_1,std::placeholders::_2));
    conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete,this,std::placeholders::_1));

    conns_[conn->fd()] = conn;
    //printf ("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());


    if(newconnectioncb_) newconnectioncb_(conn);         //回调EchoServer::HandleNewConnection()
}

void TcpServer::closeconnection(spConnection conn)//关闭客户端的连接，在Connection类中回调此函数
{
    //先回调EchoServer::HandleClose()，再关闭连接
    if(closeconnectioncb_)  closeconnectioncb_(conn);
    
    //printf("client(eventfd=%d) disconnected.\n",conn->fd());
    conns_.erase(conn->fd());
    //delete conn;
}

void TcpServer::errorconnection(spConnection conn)//客户端的连接错误，在Connection类中回调此函数
{
    //回调EchoServer::HandleError()
    if(errorconnectioncb_) errorconnectioncb_(conn);

    //printf("client(eventfd=%d) error.\n",conn->fd());
    conns_.erase(conn->fd());
    //delete conn;
}


void TcpServer::onmessage(spConnection conn,std::string& message)  //处理客户端的请求报文，在Connection类中回调此函数
{
    if(onmessagecb_) onmessagecb_(conn,message); // 回调EchoServer::HandleMessage()
}

void TcpServer::sendcomplete(spConnection conn)    //数据发送完成后的回调
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
void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    newconnectioncb_=fn;
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    closeconnectioncb_=fn;
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    errorconnectioncb_=fn;
}

void TcpServer::setonmessagecb(std::function<void(spConnection,std::string &message)> fn)
{
    onmessagecb_=fn;
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_=fn;
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    timeoutcb_=fn;
}
////////////////////////////////////////////////////////////////////////////////////////////////