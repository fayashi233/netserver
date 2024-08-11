#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port)
        :loop_(loop)
{
    servsock_ = new Socket(createnonblocking()); 
    InetAddress servaddr(ip,port);
    servsock_->setkeepalive(true);
    servsock_->setreuseaddr(true);
    servsock_->setreuseport(true);
    servsock_->settcpnodelay(true);
    servsock_->bind(servaddr);
    servsock_->listen();

    acceptchannel_ = new Channel(loop_,servsock_->fd());
    //使用回调函数，绑定一个函数给channel
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconnection,this));   
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}
#include "Connection.h"
void Acceptor::newconnection()//处理新客户端连接请求
{
    InetAddress clientaddr;

    Socket *clientsock = new Socket(servsock_->accept(clientaddr));

    clientsock->setipport(clientaddr.ip(),clientaddr.port());

    newconnectioncb_(clientsock);
}
void Acceptor::setnewconnectioncb(std::function<void(Socket*)> fn) 
{
    newconnectioncb_ = fn;
}