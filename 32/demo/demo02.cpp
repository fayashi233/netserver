#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>

int main(int argc, char const *argv[])
{
    int pipefd[2];      //无名管道，有两个fd，表示管道的两端，fd[0]用于读，fd[1]用于写
    if(pipe2(pipefd,0)==-1)    //O_CLOEXEC|O_NONBLOCK  //在子进程中不继承管道|非阻塞的管道
    {
        printf("创建管道失败\n");
    }

    write(pipefd[1],"hello world",11);

    ssize_t ret;
    char buf[128] = {0};

    //从管道中读取数据
    ret = read(pipefd[0],&buf,sizeof(buf));
    printf("ret=%d,buf=%s\n",ret,buf);


    ret = read(pipefd[0],&buf,sizeof(buf));
    printf("ret=%d,buf=%s\n",ret,buf);

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
