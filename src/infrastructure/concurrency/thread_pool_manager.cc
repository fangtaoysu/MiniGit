#include "thread_pool_manager.h"

namespace minigit::infrastructure::concurrency {

ThreadPoolManager::ThreadPoolManager(const size_t thread_count)
    : pool_(thread_count > 0 ? thread_count
                             : std::thread::hardware_concurrency()) {}

ThreadPoolManager::~ThreadPoolManager() {
    // The BS::thread_pool destructor automatically waits for tasks,
    // so no explicit wait call is needed here for graceful shutdown.
}

}  // namespace minigit::infrastructure::concurrency