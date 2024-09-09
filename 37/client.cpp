// 网络通讯的客户端程序。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/signal.h>
#include <signal.h>
#include <iostream>


// 忽略关闭全部的信号、关闭全部的IO，缺省只忽略信号，不关IO。 
// 不希望后台服务程序被信号打扰，需要什么信号可以在程序中设置。
// 实际上关闭的IO是0、1、2。
void closeioandsignal(bool bcloseio)
{
    int ii=0;

    for (ii=0;ii<64;ii++)
    {
        if (bcloseio==true) close(ii);

        signal(ii,SIG_IGN); 
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage:./client ip port\n"); 
        printf("example:./client 192.168.150.128 5085\n\n"); 
        return -1;
    }

    //closeioandsignal(true);

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
 
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket() failed.\n"); return -1; }
    
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2]));
    servaddr.sin_addr.s_addr=inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
    }

    printf("connect ok.\n");
    printf("开始时间：%d\n",time(0));
    

    for (int ii=0;ii<100000;ii++)
    {
        // 从命令行输入内容。
        memset(buf,0,sizeof(buf));
        //printf("please input:"); scanf("%s",buf);
        sprintf(buf,"这是第%d条消息",ii+1);

        char tmpbuf[1024];
        memset(tmpbuf,0,sizeof(tmpbuf));
        int len = strlen(buf);
        memcpy(tmpbuf,&len,4);
        memcpy(tmpbuf+4,buf,len);


        //printf("send: %s\n",buf);
        send(sockfd,tmpbuf,len+4,0);
        
        // if (send(sockfd,tmpbuf,len+4,0) <=0)       // 把命令行输入的内容发送给服务端。
        // { 
        //     printf("write() failed.\n");  close(sockfd);  return -1;
        // }
        

    //}

    //for (int ii=0;ii<1;ii++){

        //int len;
        recv(sockfd,&len,4,0);
        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,len,0); 
        
        //printf("read() failed.\n");  close(sockfd);  return -1;
    
        //printf("recv:%s\n",buf);

    }

    

    // for(int ii = 0;ii<10;ii++)
    // {
    //     memset(buf,0,sizeof(buf));
    //     sprintf(buf,"这是第%d条信息",ii+1);
    //     send(sockfd,buf,strlen(buf),0);
    //     memset(buf,0,sizeof(buf));

    //     recv(sockfd,buf,1024,0);

    //     printf("recv:%s\n",buf);
    //     sleep(1);
    // }



    printf("结束时间：%d\n",time(0));
} 