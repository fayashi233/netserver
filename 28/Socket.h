#pragma once
#include <sys/socket.h>
#include <sys/types.h>          
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "InetAddress.h"

//创建一个非阻塞的socket
int createnonblocking();

//socket类
class Socket
{
private:
    const int fd_;  //Socket持有的fd
    std::string ip_;    // 如果是listenfd，存放服务端监听的ip，如果是客户端连接的ip，存放对端的ip
    uint16_t port_;     // 如果是listenfd，存放服务端监听的端口，如果是客户端连接的ip，存放外部端口
    

public:
    Socket(int fd);
    Socket();
    ~Socket();


    int fd() const;             //返回fd_成员
    std::string ip() const;
    uint16_t port() const;

    void setipport(const std::string &ip, uint16_t port);           //设置ip_和port_成员

    void setreuseaddr(bool on); //设置SO_REUSEADDR选项
    void setreuseport(bool on); //设置SO_REUSEPORT选项
    void settcpnodelay(bool on); //设置TCPNODELAY选项
    void setkeepalive(bool on); //设置SO_KEEPALIVE选项

    void bind(const InetAddress& servaddr);     //服务端的socket将调用此函数
    void listen(int n = 128);                   //服务端的socket将调用此函数
    int accept(InetAddress &clientaddr);       //服务端的socket将调用此函数，返回fd_


};