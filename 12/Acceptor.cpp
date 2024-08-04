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
    acceptchannel_->setreadcallback(std::bind(&Channel::newconnection,acceptchannel_,servsock_));
    acceptchannel_->enablereading();
}

Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}
