#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>

// 设置非阻塞的IO。
void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.150.128 5085\n\n"); 
        return -1; 
    }

    // 创建服务端用于监听的listenfd。
    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenfd < 0)
    {
        perror("socket() failed"); return -1;
    }

    // 设置listenfd的属性。
    int opt = 1; 
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));   
    setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));  
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));  
    setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt)); 

    setnonblocking(listenfd);    // 设置非阻塞。

    struct sockaddr_in servaddr;  
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(argv[1]); 
    servaddr.sin_port = htons(atoi(argv[2])); 

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind() failed"); close(listenfd); return -1;
    }

    if (listen(listenfd, 128) != 0 )  
    {
        perror("listen() failed"); close(listenfd); return -1;
    }

    int epollfd = epoll_create(1); 

    struct epoll_event ev; 
    ev.data.fd = listenfd; 
    ev.events = EPOLLIN; 

    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev); 

    struct epoll_event evs[10]; 

    while (true) 
    {
        int infds = epoll_wait(epollfd, evs, 10, -1); 

        if (infds < 0)
        {
            perror("epoll_wait() failed"); break;
        }

        for (int ii = 0; ii < infds; ii++) 
        {
            if (evs[ii].data.fd == listenfd) 
            {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
                setnonblocking(clientfd);  

                printf ("accept client(fd=%d, ip=%s, port=%d) ok.\n", clientfd, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

                ev.data.fd = clientfd;
                ev.events = EPOLLIN | EPOLLET; 
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
            }
            else 
            {
                if (evs[ii].events & EPOLLRDHUP) 
                {
                    printf("client(eventfd=%d) disconnected.\n", evs[ii].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, evs[ii].data.fd, NULL); 
                    close(evs[ii].data.fd); 
                }
                else if (evs[ii].events & (EPOLLIN | EPOLLPRI))  
                {
                    char buffer[1024];
                    while (true) 
                    {    
                        bzero(&buffer, sizeof(buffer));

                        // 1. 读取前4字节（消息长度）
                        ssize_t nread = recv(evs[ii].data.fd, buffer, 4, 0);
                        if (nread <= 0) break; 

                        int msglen;
                        memcpy(&msglen, buffer, sizeof(int));  // 提取消息长度

                        // 2. 根据消息长度读取完整消息
                        nread = recv(evs[ii].data.fd, buffer, msglen, 0);
                        if (nread > 0) 
                        {
                            //printf("recv(eventfd=%d): %s\n", evs[ii].data.fd, buffer);

                            // 将接收到的消息发回客户端
                            char sendbuf[1024];
                            int sendlen = strlen(buffer);
                            memcpy(sendbuf, &sendlen, sizeof(int));  // 添加4字节长度
                            memcpy(sendbuf + 4, buffer, sendlen);     // 添加消息数据

                            send(evs[ii].data.fd, sendbuf, sendlen + 4, 0);
                        } 
                        else 
                        {
                            if (nread == 0) 
                            {
                                printf("client(eventfd=%d) disconnected.\n", evs[ii].data.fd);
                                close(evs[ii].data.fd);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

  return 0;
}
