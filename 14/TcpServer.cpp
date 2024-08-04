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

}

TcpServer::~TcpServer()
{
    delete acceptor_; 
}

void TcpServer::start()
{
    loop_.run();
}
