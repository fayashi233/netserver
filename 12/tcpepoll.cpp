/*
 * 程序名：tcpepoll.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
*/
/*
#include <stdio.h>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"*/
#include "TcpServer.h"
int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    // Socket servsock;
    // InetAddress servaddr(argv[1],atoi(argv[2]));
    // servsock.setkeepalive(true);
    // servsock.setreuseaddr(true);
    // servsock.setreuseport(true);
    // servsock.settcpnodelay(true);
    // servsock.bind(servaddr);
    // servsock.listen();

    // EventLoop loop;
    // Channel *servchannel = new Channel(&loop,servsock.fd());
    // //使用回调函数，绑定一个函数给channel
    // servchannel->setreadcallback(std::bind(&Channel::newconnection,servchannel,&servsock));
    // servchannel->enablereading();
    TcpServer tcpserver(argv[1],atoi(argv[2]));

    tcpserver.start();

    //loop.run();     //运行事件循环 
    return 0;
}