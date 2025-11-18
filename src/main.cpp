#include "infrastructure/config/app_config.h"
#include "infrastructure/database/mysql_connection_pool.h"
#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"
#include "infrastructure/concurrency/thread_pool_manager.h"
#include <future>
#include <iostream>

int main() {
    // 1. 初始化日志记录器
    std::filesystem::path project_root = PathUtils::GetProjectRoot();
    std::filesystem::path log_config_path = project_root / "config" / "log4cplus.properties";
    InitImLogger(log_config_path);

    // 2. 加载应用配置
    std::filesystem::path app_config_path = project_root / "config" / "config.json";
    if (!AppConfig::GetInstance().LoadConfig(app_config_path)) {
        LOG_FATAL("Failed to load application config. Exiting.");
        return 1;
    }
    LOG_INFO("Application config loaded successfully.");

    // 3. 初始化数据库连接池
    const auto& db_config = AppConfig::GetInstance().GetMySqlSettings();
    if (!MySQLConnectionPool::GetInstance().Init(db_config)) {
        LOG_FATAL("Failed to initialize MySQL connection pool. Exiting.");
        return 1;
    }
    LOG_INFO("database started successfully.");


    // 示例：尝试获取一个数据库连接
    if (db_config.enable) {
        auto conn = MySQLConnectionPool::GetInstance().GetConnection();
        if (conn) {
            LOG_INFO("Successfully got a connection from the database pool!");
            // 连接在使用完毕后会自动通过智能指针返回池中
        } else {
            LOG_ERROR("Failed to get a connection from the database pool.");
        }
    }

    // 启动线程池
    infrastructure::concurrency::ThreadPoolManager thread_pool(AppConfig::GetInstance().GetThreadPoolSettings().size);
    std::future<int> future_result = thread_pool.SubmitTask([]() {
        LOG_INFO("Task running in thread pool.");
        return 42;
    });
    LOG_INFO("线程池已启动，result: " << future_result.get());

    return 0;
}