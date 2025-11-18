#include <gtest/gtest.h>

#include "infrastructure/config/config_loader.h"
#include "infrastructure/database/mysql_connection_pool.h"
#include "shared/path_utils.h"


class DatabaseIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试开始前，确保加载了配置
        std::filesystem::path project_root = PathUtils::GetProjectRoot();
        std::filesystem::path app_config_path = project_root / "config" / "config.json";
        
        // 加载配置，这是所有数据库测试的前提
        minigit::infrastructure::config::ConfigLoader loader;
        loader.LoadConfig();
        const auto& config = loader.GetConfig();
        ASSERT_TRUE(config.mysql.enable) << "MySQL database is not enabled in config.";
    }
};

// 测试 AppConfig 是否能正确加载数据库配置
TEST_F(DatabaseIntegrationTest, ShouldLoadDatabaseConfigCorrectly) {
    minigit::infrastructure::config::ConfigLoader loader;
    loader.LoadConfig();
    const auto& db_config = loader.GetConfig().mysql;

    // 验证从 config.json 加载的值是否符合预期
    EXPECT_TRUE(db_config.enable);
    EXPECT_EQ(db_config.host, "127.0.0.1");
    EXPECT_EQ(db_config.user, "root");
    EXPECT_EQ(db_config.password, "MySQL_root");
    EXPECT_EQ(db_config.db_name, "mini_git");
    // 注意：我们不直接在测试中断言密码，因为这不是一个安全测试
    EXPECT_GT(db_config.pool_size, 0);
}

// 测试 MySQL 连接池是否能成功初始化并获取连接
TEST_F(DatabaseIntegrationTest, ShouldInitializePoolAndGetConnection) {
    // 初始化连接池
    bool pool_initialized = MySQLConnectionPool::GetInstance().Init();
    ASSERT_TRUE(pool_initialized) << "MySQL connection pool failed to initialize.";

    // 尝试获取一个连接
    auto conn = MySQLConnectionPool::GetInstance().GetConnection();
    
    // 验证我们是否成功获得了一个有效的连接对象
    ASSERT_NE(conn, nullptr) << "Failed to get a valid connection from the pool.";
    
    // 简单验证连接是否可用
    EXPECT_FALSE(conn->isClosed()) << "Connection is closed immediately after retrieval.";
}