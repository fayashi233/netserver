#include "EventLoop.h"

EventLoop::EventLoop():ep_(new Epoll),wakeupfd_(eventfd(0,EFD_NONBLOCK)),wakechannel_(new Channel(this,wakeupfd_))
{
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
    wakechannel_->enablereading();
}
EventLoop::~EventLoop()
{
    //delete ep_;
}

void EventLoop::run()
{
    //printf("EventLoop::run() thread is %d.\n",syscall(SYS_gettid));

    threadid_ = syscall(SYS_gettid);

    while (true)        // 事件循环。
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
    printf("EventLoop::handlewakeup() thread id is %d\n",syscall(SYS_gettid));

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