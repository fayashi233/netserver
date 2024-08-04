#include "TcpServer.h"

TcpServer::TcpServer(const std::string &ip,const uint16_t port)
{
    // Socket *servsock = new Socket(createnonblocking()); 
    // InetAddress servaddr(ip,port);
    // servsock->setkeepalive(true);
    // servsock->setreuseaddr(true);
    // servsock->setreuseport(true);
    // servsock->settcpnodelay(true);
    // servsock->bind(servaddr);
    // servsock->listen();

    // Channel *servchannel = new Channel(&loop_,servsock->fd());
    // //使用回调函数，绑定一个函数给channel
    // servchannel->setreadcallback(std::bind(&Channel::newconnection,servchannel,servsock));
    // servchannel->enablereading();

    acceptor_ = new Acceptor(&loop_,ip,port);
    acceptor_->setnewconnectioncb(std::bind(&TcpServer::newconnection,this,std::placeholders::_1));
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
    conns_[conn->fd()] = conn;
    printf ("new connection(fd=%d,ip=%s,port=%d) ok.\n",conn->fd(),conn->ip().c_str(),conn->port());
}