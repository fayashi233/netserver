#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum):stop_(false)
{
    //启动threadnum个线程，每个线程将阻塞在条件变量上
    for (size_t ii = 0; ii < threadnum; ii++)
    {
        //用lambda函数创建线程
        threads_.emplace_back([this]
        {
            printf("create thread(%d).\n",syscall(SYS_gettid));

            while(stop_==false)
            {
                std::function<void()> task; //存放出队的元素

                {//锁作用域开始   ////////////////////////////
                    std::unique_lock<std::mutex> lock(this->mutex_);

                    //等待生产者的条件变量
                    this->condition_.wait(lock,[this]   
                    {
                        //lambda返回true，条件变量被触发
                        return ((this->stop_==true)||(this->taskqueue_.empty()==false));
                    });

                    //在线程池停止之前，如果队列中还有任务，执行完再退出
                    if((this->stop_==true)&&(this->taskqueue_.empty()==true)) return;

                    //出队一个任务
                    task = std::move(this->taskqueue_.front());
                    this->taskqueue_.pop();
                }//锁作用域结束   ////////////////////////////

                printf("thread is %d.\n",syscall(SYS_gettid));
                task();     //执行任务
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    stop_ = true;

    condition_.notify_all();    //唤醒全部线程

    //等待全部线程执行玩任务后退出
    for(std::thread &th : threads_)
        th.join();
}

//把任务添加到线程队列中
void ThreadPool::addtask(std::function<void()> task)
{
    {//锁作用域开始   ////////////////////////////
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }//锁作用域结束   ////////////////////////////

    condition_.notify_one();        //唤醒一个线程
}


