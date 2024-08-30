#include "Connection.h"

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock)
    :loop_(loop),clientsock_(std::move(clientsock)),disconnect_(false),clientchannel_(new Channel(loop_,clientsock_->fd()))
{
    //为新客户端准备读事件，并添加到epoll中
    //clientchannel_ = new Channel(loop_,clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));

    clientchannel_->useet();    //使用边缘触发
    clientchannel_->enablereading();
}

Connection::~Connection()
{
    //delete clientchannel_;
    //delete clientsock_;
    //printf("Connection 对象已析构\n");
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
    disconnect_=true;
    clientchannel_->remove();
    closecallback_(shared_from_this());
}
void Connection::errorcallback()       //TCP连接错误时的回调函数，供Channel回调
{
    disconnect_=true;
    clientchannel_->remove();
    errorcallback_(shared_from_this());
}


//#include<sys/syscall.h>


void Connection::writecallback()                   //处理写事件的回调函数，供Channel回调
{
    int writen = ::send(fd(),outputbuffer_.data(),outputbuffer_.size(),0);  //尝试把发送缓冲区中的数据全部发送出去
    if(writen > 0) outputbuffer_.erase(0,writen);

// printf("Connection::writecallback() thread is %d.\n",syscall(SYS_gettid));

    //如果发送缓冲区中没有数据了，表示数据已经发送成功，不再关注写事件
    if(outputbuffer_.size()==0) 
    {
        clientchannel_->disablewriting();
        sendcompletecallback_(shared_from_this());
    }
}

void Connection::setclosecallback(std::function<void(spConnection)> fn)//设置关闭fd_的回调函数
{
    closecallback_ = fn;
}

void Connection::seterrorcallback(std::function<void(spConnection)> fn)//设置fd_发生错误的回调函数
{
    errorcallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(spConnection,std::string&)> fn)//设置处理报文的回调函数
{
    onmessagecallback_ = fn;
}

void Connection::setsendcompletecallback(std::function<void(spConnection)> fn)
{
    sendcompletecallback_ = fn;
}



void Connection::send(const char *data,size_t size)        //发送数据
{

    if(disconnect_==true) {printf("客户端断开，不发送数据\n");return;}

    //这里要使用智能指针，防止message栈对象在队列任务执行前被释放
    std::shared_ptr<std::string> message(new std::string(data));
//    printf("Connection::send() thread is %d.\n",syscall(SYS_gettid));
    if(loop_->isinloopthread()) //判断当前线程是否为事件循环线程（IO线程）
    {
        //如果当前线程是IO线程，直接调用sendinloop()
        printf("send()在事件循环的线程中\n");
        sendinloop(message);
    }
    else
    {
        //如果不是IO线程，把发送数据的操作交给IO线程去执行
        printf("send()不在事件循环的线程中\n");
        loop_->queueinloop(std::bind(&Connection::sendinloop,this,message));
    }

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

            //把数据放在接收缓冲区中，当全部的数据已读取完毕后再处理
            inputbuffer_.append(buffer,nread);


        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {

            while(true)
            {
                int len;
                memcpy(&len,inputbuffer_.data(),4);
                if(inputbuffer_.size()<len+4) break;        //如果接收缓冲区中的数据量少于报文头部，说明接收缓冲区的报文内容不完整

                std::string message(inputbuffer_.data()+4,len); //从inputbuffer中获取一个报文
                inputbuffer_.erase(0,len+4);                //从inputbuffer中删除已经获取的报文
                printf("message (eventfd=%d):%s\n",fd(),message.c_str());

                //更新时间戳
                lastatime_=Timestamp::now();
                //std::cout << "lastatime=" << lastatime_.tostring() << std::endl;

                onmessagecallback_(shared_from_this(),message);   //回调TcpServer::onmessage()

            }
            break;

        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            //clientchannel_->remove();   //从事件循环中删除Channel
            closecallback();        //回调TcpServer::closecallback()
            break;
        }
    }
}


//发送数据，如果当前线程是IO线程，直接调用此函数，如果是工作线程，将此函数传给IO线程
void Connection::sendinloop(std::shared_ptr<std::string> data)
{
    outputbuffer_.appendwithhead(data->data(),data->size());
    //注册写事件
    clientchannel_->enablewriting();
}


