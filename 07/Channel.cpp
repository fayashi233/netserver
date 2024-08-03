#include "Channel.h"
Channel::Channel(Epoll *ep, int fd, bool islisten) : ep_(ep), fd_(fd), islisten_(islisten)
{
}

Channel::~Channel()
{
}

int Channel::fd() // 返回fd_成员
{
    return fd_;
}

void Channel::useet() // 采用边缘触发
{
    events_ = events_ | EPOLLET;
}

void Channel::enablereading() // 让epoll_wait()监视fd_的读事件
{
    events_ = events_ | EPOLLIN;
    ep_->updatechannel(this);
}

void Channel::setinepoll() // 把inepoll_成员的值设置为true
{
    inepoll_ = true;
}

void Channel::setrevent(uint32_t ev) // 设置revents_的值为ev
{
    revents_ = ev;
}

bool Channel::inpoll() // 返回inepoll成员
{
    return inepoll_;
}

uint32_t Channel::events() // 返回events_成员
{
    return events_;
}

uint32_t Channel::revents() // 返回revents_成员
{
    return revents_;
}

void Channel::handleevent(Socket *servsock) // 事件处理函数，epoll_wait()返回的时候，执行它
{
    ////////////////////////////////////////////////////////////////////////
    if (revents_ & EPOLLRDHUP)                // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
    {
        printf("client(eventfd=%d) disconnected.\n",fd_);
        //epoll_ctl(epollfd, EPOLL_CTL_DEL, ev.data.fd, NULL); // 从epoll集合中删除该fd
        close(fd_);            // 关闭客户端的fd。
    }                                //  普通数据  带外数据
    else if (revents_ & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
    {
        if (islisten_ == true)   // 如果是listenfd有事件，表示有新的客户端连上来。
        {
            ////////////////////////////////////////////////////////////////////////
            
            InetAddress clientaddr;

            //std::unique_ptr<Socket> clientsock(new Socket(servsock.accept(clientaddr)));

            Socket *clientsock = new Socket(servsock->accept(clientaddr));


            printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientsock->fd(),clientaddr.ip(),clientaddr.port());

            

            //ep.addfd(clientsock->fd(),EPOLLIN|EPOLLET);     //客户端连上来的fd采用边缘触发
            Channel *clientchannel = new Channel(ep_,clientsock->fd(),false);
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
                ssize_t nread = read(fd_, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                if (nread > 0)      // 成功的读取到了数据。
                {
                    // 把接收到的报文内容原封不动的发回去。
                    printf("recv(eventfd=%d):%s\n",fd_,buffer);
                    send(fd_,buffer,strlen(buffer),0);
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
                    printf("client(eventfd=%d) disconnected.\n",fd_);
                    close(fd_);            // 关闭客户端的fd。
                    break;
                }
            }
        }
    }
    else if (revents_ & EPOLLOUT)             // 有数据需要写，暂时没有代码，以后再说。
    {
    }
    else                                            // 其它事件，都视为错误。
    {
        printf("client(eventfd=%d) error.\n",fd_);
        close(fd_);            // 关闭客户端的fd
    }
            ////////////////////////////////////////////////////////////////////////
}
