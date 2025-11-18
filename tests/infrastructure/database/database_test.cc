#include <gtest/gtest.h>
#include "infrastructure/config/app_config.h"
#include "infrastructure/database/database_manager.h"

using namespace infrastructure::database;

class DatabaseManagerTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // 在所有测试开始前，加载配置并初始化 DbManager
        bool config_loaded = AppConfig::GetInstance().LoadConfig(CONFIG_FILE_PATH);
        ASSERT_TRUE(config_loaded) << "Failed to load application config for testing.";
        
        const auto& db_config = AppConfig::GetInstance().GetMySqlSettings();
        ASSERT_TRUE(db_config.enable) << "Database is not enabled in the test config.";

        bool db_initialized = DbManager::GetInstance().Initialize(db_config);
        ASSERT_TRUE(db_initialized) << "Database Manager failed to initialize.";
    }

    void SetUp() override {
        // 在每个测试开始前，确保我们有一个干净的测试表
        const std::string create_table_sql = 
            "CREATE TABLE IF NOT EXISTS test_users ("
            "id INT AUTO_INCREMENT PRIMARY KEY,"
            "name VARCHAR(255) NOT NULL,"
            "email VARCHAR(255) UNIQUE NOT NULL"
            ")";
        DbManager::GetInstance().Execute(create_table_sql);
        DbManager::GetInstance().Execute("DELETE FROM test_users");
    }

    void TearDown() override {
        // 在每个测试结束后，清理测试表
        DbManager::GetInstance().Execute("DELETE FROM test_users");
    }
};

// 测试基本的插入和查询操作
TEST_F(DatabaseManagerTest, ShouldExecuteInsertAndQuery) {
    // 插入
    int64_t affected_rows = DbManager::GetInstance().Execute(
        "INSERT INTO test_users (name, email) VALUES (?, ?)", {"test_user", "test@example.com"}
    );
    ASSERT_EQ(affected_rows, 1);

    // 查询
    auto results = DbManager::GetInstance().Query(
        "SELECT name, email FROM test_users WHERE email = ?", {"test@example.com"}
    );
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]["name"], "test_user");
    EXPECT_EQ(results[0]["email"], "test@example.com");
}

// 测试更新操作
TEST_F(DatabaseManagerTest, ShouldExecuteUpdate) {
    DbManager::GetInstance().Execute(
        "INSERT INTO test_users (name, email) VALUES (?, ?)", {"initial_name", "update@example.com"}
    );

    // 更新
    int64_t affected_rows = DbManager::GetInstance().Execute(
        "UPDATE test_users SET name = ? WHERE email = ?", {"updated_name", "update@example.com"}
    );
    ASSERT_EQ(affected_rows, 1);

    // 验证更新
    auto results = DbManager::GetInstance().Query(
        "SELECT name FROM test_users WHERE email = ?", {"update@example.com"}
    );
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]["name"], "updated_name");
}

// 测试删除操作
TEST_F(DatabaseManagerTest, ShouldExecuteDelete) {
    DbManager::GetInstance().Execute(
        "INSERT INTO test_users (name, email) VALUES (?, ?)", {"to_delete", "delete@example.com"}
    );

    // 删除
    int64_t affected_rows = DbManager::GetInstance().Execute(
        "DELETE FROM test_users WHERE email = ?", {"delete@example.com"}
    );
    ASSERT_EQ(affected_rows, 1);

    // 验证删除
    auto results = DbManager::GetInstance().Query(
        "SELECT name FROM test_users WHERE email = ?", {"delete@example.com"}
    );
    ASSERT_TRUE(results.empty());
}

// 测试事务提交
TEST_F(DatabaseManagerTest, ShouldCommitTransaction) {
    DbManager::GetInstance().BeginTransaction();
    DbManager::GetInstance().Execute(
        "INSERT INTO test_users (name, email) VALUES (?, ?)", {"tx_commit", "commit@example.com"}
    );
    DbManager::GetInstance().CommitTransaction();

    // 在事务外部验证数据是否存在
    auto results = DbManager::GetInstance().Query(
        "SELECT name FROM test_users WHERE email = ?", {"commit@example.com"}
    );
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0]["name"], "tx_commit");
}

// 测试事务回滚
TEST_F(DatabaseManagerTest, ShouldRollbackTransaction) {
    DbManager::GetInstance().BeginTransaction();
    DbManager::GetInstance().Execute(
        "INSERT INTO test_users (name, email) VALUES (?, ?)", {"tx_rollback", "rollback@example.com"}
    );
    DbManager::GetInstance().RollbackTransaction();

    // 在事务外部验证数据是否不存在
    auto results = DbManager::GetInstance().Query(
        "SELECT name FROM test_users WHERE email = ?", {"rollback@example.com"}
    );
    ASSERT_TRUE(results.empty());
}

// 测试无效查询
TEST_F(DatabaseManagerTest, ShouldHandleInvalidQueryGracefully) {
    // 这个查询会因为表名错误而失败
    auto results = DbManager::GetInstance().Query("SELECT * FROM non_existent_table");
    // 期望返回一个空的结果集，而不是抛出异常或崩溃
    ASSERT_TRUE(results.empty());
}

// 测试无效执行
TEST_F(DatabaseManagerTest, ShouldHandleInvalidExecuteGracefully) {
    // 这个执行会因为语法错误而失败
    int64_t affected_rows = DbManager::GetInstance().Execute("INSERTT INTO test_users (name) VALUES ('fail')");
    // 期望返回 -1，而不是抛出异常或崩溃
    ASSERT_EQ(affected_rows, -1);
}