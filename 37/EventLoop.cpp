#include "EventLoop.h"

int createtimerfd(int sec=30)
{
    int tfd=timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC|TFD_NONBLOCK);   // 创建timerfd。TFD_CLOEXEC子进程不要继承父进程的fd
    struct itimerspec timeout;                                // 定时时间的数据结构。
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = sec;                             // 定时时间为5秒。
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(tfd,0,&timeout,0);                  // 开始计时。alarm(5)
    return tfd;
}


EventLoop::EventLoop(int mainloop,int timetvl,int timeout):
                    ep_(new Epoll),wakeupfd_(eventfd(0,EFD_NONBLOCK))
                    ,wakechannel_(new Channel(this,wakeupfd_)),stop_(false),
                   timerfd_(createtimerfd(timeout)),timerchannel_(new Channel(this,timerfd_)),mainloop_(mainloop),
                   timetvl_(timetvl),timeout_(timeout)
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakechannel_->enablereading();

    timerchannel_->setreadcallback(std::bind(&EventLoop::handletimer,this));
    timerchannel_->enablereading();
    
}
EventLoop::~EventLoop()
{
    //delete ep_;
}

void EventLoop::run()
{
    threadid_ = syscall(SYS_gettid);

    while (stop_==false)        // 事件循环。
    {
        std::vector<Channel*> channels = ep_->loop(10*1000);       //存放epoll_wait()返回的事件

        //如果channels为空，表示超时，回调TcpServer::epolltimeout()
        if(channels.size()==0)
        {
            epolltimeoutcallback_(this);
        }

        for(auto &ch : channels)
        {
            ch->handleevent();
        }
    }
}

void EventLoop::stop()    //停止事件循环
{
    stop_ = true;
    //让epoll_wait返回
    wakeup();
}

void EventLoop::updatechannel(Channel *ch)        //把channel 添加/更新 到红黑树上
{
    ep_->updatechannel(ch);
}

void EventLoop::removechannel(Channel *ch)        //从红黑树上删除Channel
{
    ep_->removechannel(ch);
}


void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)   //设置epoll_wait()超时回调函数
{
    epolltimeoutcallback_ = fn;
}

bool EventLoop::isinloopthread()      //判断当前线程是否为事件循环线程
{
    return syscall(SYS_gettid) == threadid_;
}

void EventLoop::queueinloop(std::function<void()> fn)  //将任务添加到队列中
{
    {
        std::lock_guard<std::mutex> gd(mutex_);
        taskqueue_.push(fn);
    }

    // 唤醒事件循环
    wakeup();
}

//用eventfd唤醒事件循环
void EventLoop::wakeup()      
{
    uint64_t val = 1;
    write(wakeupfd_,&val,sizeof(val));
}

void EventLoop::handlewakeup()    //事件循环被eventfd唤醒后执行的函数
{

    uint64_t val;
    read(wakeupfd_,&val,sizeof(val));   //如果不读取，eventfd的读事件会一直触发

    std::function<void()> fn;
    
    std::lock_guard<std::mutex> gd(mutex_);

    while(taskqueue_.size()>0)
    {
        fn = std::move(taskqueue_.front());  
        taskqueue_.pop();
        fn();//执行任务
    }
}


void EventLoop::handletimer()     // 闹钟响时执行的函数
{
    //重新计时
    struct itimerspec timeout;                                // 定时时间的数据结构。
    memset(&timeout,0,sizeof(struct itimerspec));
    timeout.it_value.tv_sec = timetvl_;                             // 定时时间为5秒。
    timeout.it_value.tv_nsec = 0;
    timerfd_settime(timerfd_,0,&timeout,0);                  // 开始计时。alarm(5)

    if(mainloop_)
    {
        //printf("主事件循环闹钟时间到了\n");
    }
    else
    {
        
        time_t now = time(0);
        for (auto it=conns_.begin();it!=conns_.end();)
        {
            if (it->second->timeout(now,timeout_)) 
            {
                timercallback_(it->first);             // 从TcpServer的map中删除超时的conn。
                std::lock_guard<std::mutex> gd(mmutex_);
                it = conns_.erase(it);                // 从EventLoop的map中删除超时的conn。
            } else it++;
        }
        //printf("\n");

    }
}

void EventLoop::newconnection(spConnection conn)  //把Connection对象保存在conns_
{
    {
        std::lock_guard<std::mutex> gd(mmutex_);
        conns_[conn->fd()]=conn;
    }
}

void EventLoop::settimercallback(std::function<void(int)> fn)
{
    timercallback_ = fn;
}
