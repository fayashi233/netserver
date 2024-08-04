#include "Socket.h"
int createnonblocking()
{
    int listenfd = socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK,IPPROTO_TCP);
    if(listenfd < 0)
    {
        printf("%s:%s:%d listen socket create error:%d\n",__FILE__,__FUNCTION__,__LINE__,errno);exit(-1);
    }
    return listenfd;
}

Socket::Socket(int fd) : fd_(fd)
{

}

Socket::Socket() : fd_(createnonblocking())
{
}

Socket::~Socket()
{
    ::close(fd_);
}

int Socket::fd() const
{
    return fd_;
}

void Socket::setreuseaddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&optval,static_cast<socklen_t>(sizeof optval));
}

void Socket::setreuseport(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&optval,static_cast<socklen_t>(sizeof optval));
}

void Socket::settcpnodelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_,SOL_SOCKET,TCP_NODELAY,&optval,static_cast<socklen_t>(sizeof optval));
}

void Socket::setkeepalive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(fd_,SOL_SOCKET,SO_KEEPALIVE,&optval,static_cast<socklen_t>(sizeof optval));
}

void Socket::bind(const InetAddress &servaddr)
{
    if (::bind(fd_,servaddr.addr(),sizeof(sockaddr)) < 0 )
    {
        perror("bind() failed"); ::close(fd_); exit(-1);
    }
}

void Socket::listen(int n)
{
    if (::listen(fd_,n) != 0 )        // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed"); ::close(fd_); exit(-1);
    }
}

int Socket::accept(InetAddress &clientaddr)
{
    sockaddr_in peeraddr;
    socklen_t len = sizeof(peeraddr);

    int clientfd = accept4(fd_,(sockaddr*)&peeraddr,&len,SOCK_NONBLOCK);
    //setnonblocking(clientfd);         // 客户端连接的fd必须设置为非阻塞的。
    clientaddr.setaddr(peeraddr);
    return clientfd;
}
