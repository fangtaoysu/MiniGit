#include "infrastructure/concurrency/thread_pool_manager.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

using namespace minigit::infrastructure::concurrency;

// Test fixture for ThreadPoolManager tests
class ThreadPoolManagerTest : public ::testing::Test {};

// 测试点 1: 基本构造与析构
// 验证线程池可以被成功创建（使用指定线程数或默认线程数）和销毁，而不会引发异常。
TEST_F(ThreadPoolManagerTest, ShouldConstructAndDestructWithoutErrors) {
    ASSERT_NO_THROW({ ThreadPoolManager pool(4); })
        << "Construction with a specific thread count failed.";

    ASSERT_NO_THROW({ ThreadPoolManager pool; })
        << "Default construction failed.";
}

// 测试点 2: 提交带返回值的任务
// 验证可以向线程池提交一个函数，并能通过返回的 std::future 成功获取其返回值。
TEST_F(ThreadPoolManagerTest, ShouldSubmitTaskAndReceiveCorrectResult) {
    ThreadPoolManager pool(2);
    auto future = pool.SubmitTask([]() { return 42; });
    ASSERT_EQ(future.get(), 42);
}

// 测试点 3: 提交无返回值的任务 (void)
// 验证可以提交一个没有返回值的任务，并能确认它确实被执行了。
// 我们使用 std::promise 来从任务内部发出信号。
TEST_F(ThreadPoolManagerTest, ShouldExecuteVoidTask) {
    ThreadPoolManager pool(1);
    std::promise<void> task_ran_promise;
    std::future<void> task_ran_future = task_ran_promise.get_future();

    pool.SubmitTask([&task_ran_promise]() { task_ran_promise.set_value(); });

    // 等待任务发出完成信号，设置一个超时时间以防万一。
    auto status = task_ran_future.wait_for(std::chrono::seconds(1));
    ASSERT_EQ(status, std::future_status::ready)
        << "Void task did not complete within the timeout.";
}

// 测试点 4: 并发处理多个任务
// 验证线程池可以同时处理大量的任务，并且所有任务最终都会被执行。
TEST_F(ThreadPoolManagerTest, ShouldHandleMultipleConcurrentTasks) {
    const int num_threads = 4;
    const int num_tasks = 100;
    ThreadPoolManager pool(num_threads);
    std::atomic<int> counter = 0;
    std::vector<std::future<void>> futures;

    futures.reserve(num_tasks);
    for (int i = 0; i < num_tasks; ++i) {
        futures.push_back(pool.SubmitTask([&counter]() {
            // 模拟少量工作
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            counter++;
        }));
    }

    // 等待所有任务完成
    for (auto& f : futures) {
        f.get();
    }

    // 验证计数器的值是否等于任务总数
    ASSERT_EQ(counter.load(), num_tasks);
}

// 测试点 5: 优雅停机 (Graceful Shutdown)
// 验证当 ThreadPoolManager
// 对象被销毁时，它会等待所有正在运行的任务完成后再退出。
TEST_F(ThreadPoolManagerTest, ShouldWaitForTasksToCompleteOnDestruction) {
    std::atomic<bool> long_task_completed = false;

    {
        ThreadPoolManager pool(1);
        // 提交一个耗时较长的任务
        pool.SubmitTask([&long_task_completed]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            long_task_completed = true;
        });
        // 在这里，pool 对象将被销毁。其析构函数应该会阻塞，直到任务完成。
    }

    // 如果析构函数正确地等待了，那么当我们执行到这里时，任务一定已经完成了。
    ASSERT_TRUE(long_task_completed.load());
}

// 测试点 6: 正确传递任务参数
// 验证各种类型的参数（值、引用、常量引用）都能被正确地传递给任务函数。
TEST_F(ThreadPoolManagerTest, ShouldForwardArgumentsCorrectlyToTasks) {
    ThreadPoolManager pool(1);

    // 验证基本类型的值传递
    auto future_sum =
        pool.SubmitTask([](int a, int b) { return a + b; }, 10, 20);
    ASSERT_EQ(future_sum.get(), 30);

    // 验证字符串和常量引用传递
    auto future_concat = pool.SubmitTask(
        [](const std::string& a, const std::string& b) { return a + b; },
        "hello", " world");
    ASSERT_EQ(future_concat.get(), "hello world");

    // 验证通过 std::ref 传递引用，并修改外部变量
    int value_to_modify = 10;
    auto future_modify =
        pool.SubmitTask([](int& x) { x = 100; }, std::ref(value_to_modify));
    future_modify.get();
    ASSERT_EQ(value_to_modify, 100);
}