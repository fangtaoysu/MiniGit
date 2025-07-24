#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <queue>
#include <condition_variable>
#include <type_traits>


class ThreadPool {
    public:
        ThreadPool(size_t thread_number);

        template <class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
        ~ThreadPool()=default;
        void wait();

    private: 
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        std::mutex queue_mutex_;
        std::condition_variable condition_;
        bool stop_;
};

template <class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    ); 
    // 获取任务的future，用于获取执行结果
    std::future<return_type> res = task->get_future();
    // 为什么加花括号 - 界定临界区范围
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (stop_) {
            std::cerr << "enqueue on stopped ThreadPool\n";
        }
        // forward实现完美转发，如果原来的值是左值，转发后还是左值，如果是右值，转发后还是右值
        tasks_.emplace(std::forward<F>(f));
    }
    condition_.notify_one(); // 通知一个等待的线程有新任务
    return res;
}


#endif