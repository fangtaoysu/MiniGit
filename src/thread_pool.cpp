#include "../include/thread_pool.h"



ThreadPool::ThreadPool(size_t thread_number)
    : stop_(false) {
    for (size_t i = 0; i < thread_number; ++i) {
        // 如果你要添加一个对象，已有就用 push_back，参数构造就用 emplace_back
        // 每个线程都执行下面的任务
        workers_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                { // 临界区开始
                    // 队列是共享资源
                    // （临界区） 加锁访问共享资源
                    std::unique_lock<std::mutex> lock(this->queue_mutex_);
                    // 等待条件的满足：有任务可执行或线程池停止
                    this->condition_.wait(lock, 
                        [this] {
                            return this->stop_ || !tasks_.empty();
                        }
                    );
                    // 如果线程已停止，而不任务队列是空，那么退出
                    if (this->stop_ && this->tasks_.empty()) {
                        return;
                    }
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                } // 临界区结束，自动释放锁
                task(); // 执行任务
            }
        });
    }
}

/**在对象销毁前，等待所有线程完成任务 */
void ThreadPool::wait() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop_ = true;
    }
    condition_.notify_all(); // 通知所有线程
    // 等待所有线程完成任务
    for (std::thread & worker : workers_) {
        worker.join();
    }
}