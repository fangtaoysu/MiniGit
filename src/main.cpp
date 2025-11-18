#include "infrastructure/config/app_config.h"
#include "infrastructure/database/database_manager.h"
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

    // 3. 初始化数据库管理器
    const auto& db_config = AppConfig::GetInstance().GetMySqlSettings();
    if (!infrastructure::database::DbManager::GetInstance().Initialize(db_config)) {
        LOG_FATAL("Failed to initialize Database Manager. Exiting.");
        return 1;
    }
    LOG_INFO("Database Manager started successfully.");


    // 示例：执行一个简单的数据库查询
    if (db_config.enable) {
        try {
            auto result = infrastructure::database::DbManager::GetInstance().Query("SELECT 1", {});
            LOG_INFO("Successfully executed a test query against the database!");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to execute test query: " << e.what());
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