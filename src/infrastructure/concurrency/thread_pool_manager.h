#pragma once

#include <functional>  // For std::bind
#include <future>

#include "third_party/BS_thread_pool.hpp"

namespace minigit::infrastructure::concurrency {

/**
 * @class ThreadPoolManager
 * @brief A manager class that encapsulates the BS::thread_pool library.
 *
 * This class provides a clean interface for submitting tasks to a thread pool
 * whose size is configured externally. It ensures the graceful shutdown of
 * the pool upon destruction.
 */
class ThreadPoolManager {
public:
    /**
     * @brief Constructs the ThreadPoolManager and initializes the underlying
     * thread pool.
     * @param thread_count The number of threads to create in the pool. Defaults
     * to the number of hardware threads if set to 0.
     */
    explicit ThreadPoolManager(const size_t thread_count = 0);

    /**
     * @brief Destructor. Waits for all submitted tasks to complete before
     * shutting down.
     */
    ~ThreadPoolManager();

    // Prevent copying and moving to ensure single ownership of the pool.
    ThreadPoolManager(const ThreadPoolManager&) = delete;
    ThreadPoolManager& operator=(const ThreadPoolManager&) = delete;
    ThreadPoolManager(ThreadPoolManager&&) = delete;
    ThreadPoolManager& operator=(ThreadPoolManager&&) = delete;

    /**
     * @brief Submits a task to the thread pool for execution.
     *
     * @tparam F The type of the function/callable to execute.
     * @tparam Args The types of the arguments to pass to the function.
     * @param func The function/callable to execute.
     * @param args The arguments to pass to the function.
     * @return A std::future that will hold the result of the function's
     * execution.
     */
    template <typename F, typename... Args>
    auto SubmitTask(F&& func,
                    Args&&... args) -> std::future<decltype(func(args...))> {
        // The BS::thread_pool library expects a single callable object (a
        // task). We use std::bind to bundle the function and its arguments
        // together into a single callable that the thread pool can execute.
        auto task =
            std::bind(std::forward<F>(func), std::forward<Args>(args)...);
        return pool_.submit_task(std::move(task));
    }

private:
    BS::thread_pool<> pool_;
};

}  // namespace minigit::infrastructure::concurrency
