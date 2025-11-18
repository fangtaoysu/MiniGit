#include <gtest/gtest.h>
#include "infrastructure/config/app_config.h"
#include "infrastructure/database/mysql_connection_pool.h"

class DatabaseIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // The CONFIG_FILE_PATH macro is passed in by CMakeLists.txt.
        // This ensures the test can always find the config file.
        bool config_loaded = AppConfig::GetInstance().LoadConfig(CONFIG_FILE_PATH);
        ASSERT_TRUE(config_loaded) << "Failed to load application config for testing.";
    }
};

// Tests if AppConfig correctly loads database settings.
TEST_F(DatabaseIntegrationTest, ShouldLoadDatabaseConfigCorrectly) {
    const auto& db_config = AppConfig::GetInstance().GetMySqlSettings();

    // Verify that the values loaded from config.json match expectations.
    EXPECT_TRUE(db_config.enable);
    EXPECT_EQ(db_config.host, "127.0.0.1");
    EXPECT_EQ(db_config.user, "root");
    EXPECT_FALSE(db_config.db_name.empty());
    EXPECT_GT(db_config.pool_size, 0);
}

// Tests if the MySQL connection pool can be initialized and a connection can be obtained.
TEST_F(DatabaseIntegrationTest, ShouldInitializePoolAndGetConnection) {
    const auto& db_config = AppConfig::GetInstance().GetMySqlSettings();
    ASSERT_TRUE(db_config.enable) << "Database is not enabled in the test config.";

    // Initialize the pool with the loaded configuration.
    bool pool_initialized = MySQLConnectionPool::GetInstance().Init(db_config);
    ASSERT_TRUE(pool_initialized) << "MySQL connection pool failed to initialize.";

    // Try to get a connection.
    auto conn = MySQLConnectionPool::GetInstance().GetConnection();
    
    // Verify that we received a valid connection object.
    ASSERT_NE(conn, nullptr) << "Failed to get a valid connection from the pool.";
    
    // A simple check to see if the connection is usable.
    EXPECT_FALSE(conn->isClosed()) << "Connection is closed immediately after retrieval.";
}