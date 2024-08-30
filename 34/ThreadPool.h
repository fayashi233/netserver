#pragma once
#include <queue>
#include <vector>
#include <mutex>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
#include <atomic>

class ThreadPool
{
private:
    std::vector<std::thread> threads_;            // 线程池中的线程
    std::queue<std::function<void()>> taskqueue_; // 任务队列
    std::mutex mutex_;                            // 任务队列同步互斥锁
    std::condition_variable condition_;           // 任务队列同步的条件变量
    std::atomic_bool stop_;                       // 在析构函数中，把stop_的值设置为false，全部的线程将退出
    std::string threadtype_;                      // 线程种类："IO"、"WORKS"
public:
    ThreadPool(size_t threadnum, const std::string& threadtype);

    //在析构函数中停止线程
    ~ThreadPool();

    //获取线程池的大小
    size_t size();

    //把任务添加到线程队列中
    void addtask(std::function<void()> task);


};

