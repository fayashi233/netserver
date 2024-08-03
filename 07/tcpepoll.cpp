/*
 * 程序名：tcpepoll.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
*/
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



int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    

    Socket servsock;
    InetAddress servaddr(argv[1],atoi(argv[2]));
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.setreuseport(true);
    servsock.settcpnodelay(true);
    servsock.bind(servaddr);
    servsock.listen();



    Epoll ep;
    //ep.addfd(servsock.fd(),EPOLLIN);     //让epoll监视listenfd的读事件，采用水平触发
    Channel *servchannel = new Channel(&ep,servsock.fd(),true);
    servchannel->enablereading();
    

    while (true)        // 事件循环。
    {
        //std::vector<epoll_event> evs;       //存放epoll_wait()返回的事件
        std::vector<Channel*> channels = ep.loop();       //存放epoll_wait()返回的事件
        //evs = ep.loop();        //等待监视的fd有事件发生

        // 如果infds>0，表示有事件发生的fd的数量。
        //for (int ii=0;ii<infds;ii++)       // 遍历epoll返回的数组evs。
        for(auto &ch : channels)
        {
               /* 
                ////////////////////////////////////////////////////////////////////////
                if (ch->revents() & EPOLLRDHUP)                // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
                {
                    printf("client(eventfd=%d) disconnected.\n",ch->fd());
                    //epoll_ctl(epollfd, EPOLL_CTL_DEL, ev.data.fd, NULL); // 从epoll集合中删除该fd
                    close(ch->fd());            // 关闭客户端的fd。
                }                                //  普通数据  带外数据
                else if (ch->revents() & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
                {
                    if (ch==servchannel)   // 如果是listenfd有事件，表示有新的客户端连上来。
                    {
                        ////////////////////////////////////////////////////////////////////////
                        
                        InetAddress clientaddr;

                        //std::unique_ptr<Socket> clientsock(new Socket(servsock.accept(clientaddr)));

                        Socket *clientsock = new Socket(servsock.accept(clientaddr));


                        printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());

                        

                        //ep.addfd(clientsock->fd(),EPOLLIN|EPOLLET);     //客户端连上来的fd采用边缘触发
                        Channel *clientchannel = new Channel(&ep,clientsock->fd(),false);
                        clientchannel->useet();
                        clientchannel->enablereading();
                        ////////////////////////////////////////////////////////////////////////
                        
                    }
                    else
                    {   // 如果是客户端连接的fd有事件。
                        char buffer[1024];
                        while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                        {    
                            bzero(&buffer, sizeof(buffer));
                            ssize_t nread = read(ch->fd(), buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                            if (nread > 0)      // 成功的读取到了数据。
                            {
                                // 把接收到的报文内容原封不动的发回去。
                                printf("recv(eventfd=%d):%s\n",ch->fd(),buffer);
                                send(ch->fd(),buffer,strlen(buffer),0);
                            } 
                            else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
                            {  
                                continue;
                            } 
                            else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                            {
                                break;
                            } 
                            else if (nread == 0)  // 客户端连接已断开。
                            {  
                                printf("client(eventfd=%d) disconnected.\n",ch->fd());
                                close(ch->fd());            // 关闭客户端的fd。
                                break;
                            }
                        }
                    }
                }
                else if (ch->revents() & EPOLLOUT)             // 有数据需要写，暂时没有代码，以后再说。
                {
                }
                else                                            // 其它事件，都视为错误。
                {
                    printf("client(eventfd=%d) error.\n",ch->fd());
                    close(ch->fd());            // 关闭客户端的fd
                }
                ////////////////////////////////////////////////////////////////////////
        }
        */
        ch->handleevent(&servsock);
        }

    }

    return 0;
}