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

// 测试eventfd

int main(int argc, char const *argv[])
{
    int efd = eventfd(2,EFD_SEMAPHORE);    //EFD_CLOEXEC|EFD_NONBLOCK|EFD_SEMAPHORE(信号量)   

    uint64_t buf = 3;
    ssize_t ret;

    //写eventfd，buf必须是8字节，写操作会累加
    ret = write(efd,&buf,sizeof(uint64_t));  //2+3=5

    //读eventfd，如果eventfd的值变成0，调用read会阻塞
    ret = read(efd,&buf,sizeof(uint64_t));
    printf("ret=%d,buf=%d\n",ret,buf);   //ret=8,buf=5

    // 读操作会清零
    //读eventfd，如果eventfd的值变成0，调用read会阻塞
    ret = read(efd,&buf,sizeof(uint64_t));
    printf("ret=%d,buf=%d\n",ret,buf);

    close(efd);

    return 0;
}
