#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    //为新客户端准备读事件，并添加到epoll中
    clientchannel_ = new Channel(loop_,clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));

    clientchannel_->useet();    //使用边缘触发
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;
}


int Connection::fd() const             //返回fd_成员
{
    return clientsock_->fd();
}
std::string Connection::ip() const
{
    return clientsock_->ip();
}
uint16_t Connection::port() const
{
    return clientsock_->port();
}

void Connection::closecallback()       //TCP连接关闭时的回调函数，供Channel回调
{
    closecallback_(this);
}
void Connection::errorcallback()       //TCP连接错误时的回调函数，供Channel回调
{
    errorcallback_(this);
}

void Connection::setclosecallback(std::function<void(Connection*)> fn)//设置关闭fd_的回调函数
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(Connection*)> fn)//设置fd_发生错误的回调函数
{
    errorcallback_ = fn;
}


void Connection::onmessage()                   //处理对端发过来的报文
{
    // 如果是客户端连接的fd有事件。
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            // printf("recv(eventfd=%d):%s\n",fd(),buffer);
            // send(fd(),buffer,strlen(buffer),0);

            //把数据放在接收缓冲区中
            inputbuffer_.append(buffer,nread);


        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            printf("recv(eventfd=%d):%s\n",fd(),inputbuffer_.data());
            //在这里，经过若干步运算
            outputbuffer_ = inputbuffer_;

            inputbuffer_.clear();

            send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);

            // send(fd(),buffer,strlen(buffer),0);
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            // printf("client(eventfd=%d) disconnected.\n",fd()));
            // close(fd());            // 关闭客户端的fd。
            closecallback();

            break;
        }
    }
}