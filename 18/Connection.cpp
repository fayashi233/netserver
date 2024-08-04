#include "Connection.h"

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop),clientsock_(clientsock)
{
    //为新客户端准备读事件，并添加到epoll中
    clientchannel_ = new Channel(loop_,clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage,clientchannel_));
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