#include <gtest/gtest.h>
#include <filesystem>
#include "../include/git_db.h"



namespace fs = std::filesystem;

class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每次测试前创建新数据库
        db_path_ = fs::temp_directory_path() / "test_git_db.db";
        db_ = std::make_unique<Database>(db_path_.string());
        
        // 创建测试表
        db_->execute(R"(
            CREATE TABLE IF NOT EXISTS objects (
                hash TEXT PRIMARY KEY,
                type TEXT NOT NULL,
                content BLOB,
                size INTEGER
            )
        )");
        
        db_->execute(R"(
            CREATE TABLE IF NOT EXISTS refs (
                name TEXT PRIMARY KEY,
                commit_hash TEXT
            )
        )");
    }

    void TearDown() override {
        // 测试结束后删除数据库文件
        db_.reset(); // 先关闭数据库
        fs::remove(db_path_);
    }

    fs::path db_path_;
    std::unique_ptr<Database> db_;
};

// 测试数据库连接和表创建
TEST_F(DatabaseTest, Initialization) {
    EXPECT_TRUE(fs::exists(db_path_));
    
    // 验证表是否存在
    auto result = db_->query("SELECT name FROM sqlite_master WHERE type='table'");
    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0]["name"], "objects");
    EXPECT_EQ(result[1]["name"], "refs");
}

// 测试 execute() 和基础 query()
TEST_F(DatabaseTest, BasicOperations) {
    // 插入数据
    db_->execute(R"(
        INSERT INTO objects (hash, type, size)
        VALUES ('abc123', 'blob', 1024)
    )");
    
    // 查询验证
    auto objects = db_->query("SELECT * FROM objects WHERE hash = 'abc123'");
    ASSERT_EQ(objects.size(), 1);
    EXPECT_EQ(objects[0]["hash"], "abc123");
    EXPECT_EQ(objects[0]["type"], "blob");
    EXPECT_EQ(objects[0]["size"], "1024"); // SQLite 返回字符串类型
}

// 测试 execute_with_params()
TEST_F(DatabaseTest, ParameterizedExecute) {
    // 参数化插入
    db_->execute_with_params(
        "INSERT INTO objects (hash, type, size) VALUES (?, ?, ?)",
        {"def456", "tree", 2048}
    );
    
    // 验证插入
    auto result = db_->query("SELECT size FROM objects WHERE hash = 'def456'");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0]["size"], "2048");
}

// 测试 query_with_params()
TEST_F(DatabaseTest, ParameterizedQuery) {
    // 准备测试数据
    db_->execute_with_params(
        "INSERT INTO objects (hash, type, size) VALUES (?, ?, ?)",
        {"xyz789", "commit", 512}
    );
    
    // 参数化查询
    auto result = db_->query_with_params(
        "SELECT type, size FROM objects WHERE hash = ? AND size > ?",
        {"xyz789", 500}
    );
    
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0]["type"], "commit");
    EXPECT_EQ(result[0]["size"], "512");
}

// 测试 NULL 值处理
TEST_F(DatabaseTest, NullHandling) {
    db_->execute_with_params(
        "INSERT INTO objects (hash, type, content) VALUES (?, ?, ?)",
        {"nulltest", "blob", nullptr}
    );
    
    auto result = db_->query("SELECT content FROM objects WHERE hash = 'nulltest'");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0]["content"], "NULL"); // 注意：SQLite 返回的 NULL 会被转为字符串 "NULL"
}

// 测试错误处理
TEST_F(DatabaseTest, ErrorHandling) {
    // 无效 SQL 语法
    EXPECT_THROW(
        db_->execute("INSERT INTO nonexistent_table VALUES (1)"),
        std::runtime_error
    );
    
    // 参数数量不匹配
    EXPECT_THROW(
        db_->execute_with_params("INSERT INTO objects (hash) VALUES (?, ?)", {"toofew"}),
        std::runtime_error
    );
}

// 测试二进制数据（可选）
TEST_F(DatabaseTest, BinaryData) {
    std::string binary_data = "\x01\x02\x03\x04";
    db_->execute_with_params(
        "INSERT INTO objects (hash, type, content) VALUES (?, ?, ?)",
        {"bin123", "blob", binary_data}
    );
    
    auto result = db_->query("SELECT content FROM objects WHERE hash = 'bin123'");
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0]["content"], binary_data);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}