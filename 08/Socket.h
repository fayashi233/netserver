#pragma once
#include <sys/socket.h>
#include <sys/types.h>          
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"




//socket类
class Socket
{
private:
    const int fd_;  //Socket持有的fd

    //创建一个非阻塞的socket
    int createnonblocking();

public:
    Socket(int fd);
    Socket();
    ~Socket();


    int fd() const;             //返回fd_成员
    void setreuseaddr(bool on); //设置SO_REUSEADDR选项
    void setreuseport(bool on); //设置SO_REUSEPORT选项
    void settcpnodelay(bool on); //设置TCPNODELAY选项
    void setkeepalive(bool on); //设置SO_KEEPALIVE选项

    void bind(const InetAddress& servaddr);     //服务端的socket将调用此函数
    void listen(int n = 128);                   //服务端的socket将调用此函数
    int accept(InetAddress &clientaddr);       //服务端的socket将调用此函数，返回fd_


};