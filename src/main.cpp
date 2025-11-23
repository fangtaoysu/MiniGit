#include <future>
#include <iostream>

#include "infrastructure/concurrency/thread_pool_manager.h"
#include "infrastructure/config/app_config.h"
#include "infrastructure/database/database_manager.h"
#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"

namespace thread_pool = minigit::infrastructure::concurrency;
namespace cfg = minigit::infrastructure::config;
namespace db = minigit::infrastructure::database;
namespace logging = minigit::infrastructure::logging;
namespace utils = minigit::shared;

int main() {
    // 1. 初始化日志记录器
    std::filesystem::path project_root = utils::GetProjectRoot();
    std::filesystem::path log_config_path =
        project_root / "config" / "log4cplus.properties";
    logging::InitImLogger(log_config_path.string());

    // 2. 加载应用配置
    std::filesystem::path app_config_path =
        project_root / "config" / "config.json";
    if (!cfg::AppConfig::GetInstance().LoadConfig(app_config_path)) {
        LOG_FATAL("Failed to load application config. Exiting.");
        return 1;
    }
    LOG_INFO("Application config loaded successfully.");

    // 3. 初始化数据库管理器
    const auto& db_config = cfg::AppConfig::GetInstance().GetMySqlSettings();
    if (!db::DbManager::GetInstance().Initialize(db_config)) {
        LOG_FATAL("Failed to initialize Database Manager. Exiting.");
        return 1;
    }
    LOG_INFO("Database Manager started successfully.");

    // 示例：执行一个简单的数据库查询
    if (db_config.enable) {
        try {
            auto result = db::DbManager::GetInstance().Query("SELECT 1", {});
            LOG_INFO(
                "Successfully executed a test query against the database!");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to execute test query: " << e.what());
        }
    }

    // 启动线程池
    thread_pool::ThreadPoolManager thread_pool(
        cfg::AppConfig::GetInstance().GetThreadPoolSettings().size);
    std::future<int> future_result = thread_pool.SubmitTask([]() {
        LOG_INFO("Task running in thread pool.");
        return 42;
    });
    LOG_INFO("线程池已启动，result: " << future_result.get());

    return 0;
}