#include "infrastructure/config/app_config.h"
#include "infrastructure/database/mysql_connection_pool.h"
#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"

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

    // 3. 初始化数据库连接池
    if (!MySQLConnectionPool::GetInstance().Init()) {
        LOG_FATAL("Failed to initialize MySQL connection pool. Exiting.");
        return 1;
    }

    LOG_INFO("Application started successfully.");

    // 示例：尝试获取一个数据库连接
    if (AppConfig::GetInstance().GetDatabaseConfig().enable) {
        auto conn = MySQLConnectionPool::GetInstance().GetConnection();
        if (conn) {
            LOG_INFO("Successfully got a connection from the pool!");
            // 连接在使用完毕后会自动通过智能指针返回池中
        } else {
            LOG_ERROR("Failed to get a connection from the pool.");
        }
    }

    return 0;
}