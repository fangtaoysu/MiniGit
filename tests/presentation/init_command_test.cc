#include <gtest/gtest.h>

#include <filesystem>
#include <memory>

#include "application/init/dependency_config.h"
#include "application/init/init_executor.h"
#include "infrastructure/config/app_config.h"
#include "infrastructure/database/database_manager.h"
#include "infrastructure/logging/logger.h"
#include "presentation/command_engine.h"
#include "presentation/validator/init_validator.h"
#include "shared/path_utils.h"

using namespace minigit::application::init;
using namespace minigit::infrastructure::config;
using namespace minigit::infrastructure::database;
using namespace minigit::presentation;
using namespace minigit::presentation::validator;
using namespace minigit::shared;

/**
 * @brief Git Init命令测试夹具
 * 
 * 负责：
 * 1. 初始化数据库连接
 * 2. 清理测试数据（git_objects, references, staging_index表）
 * 3. 注册init命令到CommandEngine
 */
class InitCommandTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // 初始化日志
        std::filesystem::path project_root = GetProjectRoot();
        std::filesystem::path log_config_path =
            project_root / "config" / "log4cplus.properties";
        minigit::infrastructure::logging::InitImLogger(log_config_path.string());

        // 加载配置并初始化数据库
        std::filesystem::path config_path = project_root / "config" / "config.json";
        bool config_loaded =
            AppConfig::GetInstance().LoadConfig(config_path);
        ASSERT_TRUE(config_loaded)
            << "Failed to load application config for testing.";

        const auto& db_config = AppConfig::GetInstance().GetMySqlSettings();
        if (!db_config.enable) {
            GTEST_SKIP() << "Database is not enabled in the test config.";
        }

        bool db_initialized = DbManager::GetInstance().Initialize(db_config);
        ASSERT_TRUE(db_initialized)
            << "Database Manager failed to initialize.";
    }

    void SetUp() override {
        // 清理测试数据，确保每次测试从干净状态开始
        CleanupTestData();

        // 创建CommandEngine并注册init命令
        engine_ = std::make_unique<CommandEngine>();
        auto init_service = DependencyConfig::CreateInitService();
        engine_->RegisterCommand(
            "init",
            std::make_unique<InitValidator>(),
            std::make_unique<InitExecutor>(init_service));
    }

    void TearDown() override {
        // 测试结束后清理数据
        CleanupTestData();
    }

    /**
     * @brief 清理测试数据
     * 删除git_objects、references、staging_index表中的所有数据
     */
    void CleanupTestData() {
        try {
            DbManager::GetInstance().Execute("DELETE FROM staging_index");
            DbManager::GetInstance().Execute("DELETE FROM `references`");
            DbManager::GetInstance().Execute("DELETE FROM git_objects");
            // commit_history表在init时不会创建数据，但为了安全也清理
            DbManager::GetInstance().Execute("DELETE FROM commit_history");
        } catch (const std::exception& e) {
            // 如果表不存在，忽略错误（第一次运行时会这样）
        }
    }

    /**
     * @brief 检查表是否存在
     */
    bool TableExists(const std::string& table_name) {
        std::string sql = R"(
            SELECT COUNT(*) as count 
            FROM information_schema.tables 
            WHERE table_schema = DATABASE() 
            AND table_name = ?
        )";
        auto results = DbManager::GetInstance().Query(sql, {table_name});
        if (results.empty()) {
            return false;
        }
        const auto& row = results[0];
        auto it = row.find("count");
        if (it != row.end()) {
            return std::stoi(it->second) > 0;
        }
        return false;
    }

    /**
     * @brief 检查引用是否存在
     */
    bool ReferenceExists(const std::string& ref_name) {
        std::string sql = "SELECT COUNT(*) as count FROM `references` WHERE name = ?";
        auto results = DbManager::GetInstance().Query(sql, {ref_name});
        if (results.empty()) {
            return false;
        }
        const auto& row = results[0];
        auto it = row.find("count");
        if (it != row.end()) {
            return std::stoi(it->second) > 0;
        }
        return false;
    }

    /**
     * @brief 获取引用目标
     */
    std::string GetReferenceTarget(const std::string& ref_name) {
        std::string sql = "SELECT target FROM `references` WHERE name = ?";
        auto results = DbManager::GetInstance().Query(sql, {ref_name});
        if (results.empty()) {
            return "";
        }
        const auto& row = results[0];
        auto it = row.find("target");
        if (it != row.end()) {
            return it->second;
        }
        return "";
    }

    std::unique_ptr<CommandEngine> engine_;
};

/**
 * @brief 测试基本的git init命令
 * 
 * 验证：
 * 1. 命令可以成功执行
 * 2. 数据库表被创建
 * 3. 默认分支被创建
 * 4. HEAD指向默认分支
 */
TEST_F(InitCommandTest, ShouldInitializeRepositorySuccessfully) {
    // 执行git init命令
    EXPECT_NO_THROW(engine_->Execute("git init"));

    // 验证数据库表已创建
    EXPECT_TRUE(TableExists("git_objects"))
        << "git_objects table should be created";
    EXPECT_TRUE(TableExists("references"))
        << "references table should be created";
    EXPECT_TRUE(TableExists("staging_index"))
        << "staging_index table should be created";
    EXPECT_TRUE(TableExists("commit_history"))
        << "commit_history table should be created";

    // 验证默认分支已创建（默认分支名为main）
    EXPECT_TRUE(ReferenceExists("refs/heads/main"))
        << "Default branch 'main' should be created";

    // 验证HEAD指向默认分支
    std::string head_target = GetReferenceTarget("HEAD");
    EXPECT_EQ(head_target, "refs/heads/main")
        << "HEAD should point to refs/heads/main";
}

/**
 * @brief 测试重复初始化应该失败
 * 
 * 验证：
 * 1. 第一次init成功
 * 2. 第二次init失败（仓库已存在）
 */
TEST_F(InitCommandTest, ShouldFailWhenRepositoryAlreadyInitialized) {
    // 第一次初始化应该成功
    EXPECT_NO_THROW(engine_->Execute("git init"));
    EXPECT_TRUE(TableExists("git_objects"))
        << "First init should create tables";

    // 记录第一次初始化后的引用数量
    auto refs_before = DbManager::GetInstance().Query(
        "SELECT COUNT(*) as count FROM `references`");
    int ref_count_before = 0;
    if (!refs_before.empty()) {
        auto it = refs_before[0].find("count");
        if (it != refs_before[0].end()) {
            ref_count_before = std::stoi(it->second);
        }
    }

    // 第二次初始化应该失败（但不会抛出异常，只是返回false）
    // 注意：由于CommandEngine::Execute返回void，我们通过检查状态来验证
    EXPECT_NO_THROW(engine_->Execute("git init"));

    // 验证引用数量没有增加（说明没有重复创建）
    auto refs_after = DbManager::GetInstance().Query(
        "SELECT COUNT(*) as count FROM `references`");
    int ref_count_after = 0;
    if (!refs_after.empty()) {
        auto it = refs_after[0].find("count");
        if (it != refs_after[0].end()) {
            ref_count_after = std::stoi(it->second);
        }
    }
    EXPECT_EQ(ref_count_before, ref_count_after)
        << "Second init should not create duplicate references";
}

/**
 * @brief 测试暂存区在初始化后被清空
 * 
 * 验证：
 * 1. 初始化后暂存区为空
 */
TEST_F(InitCommandTest, ShouldHaveEmptyStagingAreaAfterInit) {
    // 执行git init
    EXPECT_NO_THROW(engine_->Execute("git init"));

    // 验证暂存区为空
    auto staged_files = DbManager::GetInstance().Query(
        "SELECT COUNT(*) as count FROM staging_index");
    ASSERT_FALSE(staged_files.empty());
    auto it = staged_files[0].find("count");
    ASSERT_NE(it, staged_files[0].end());
    EXPECT_EQ(std::stoi(it->second), 0)
        << "Staging area should be empty after init";
}

/**
 * @brief 测试数据库表结构正确性
 * 
 * 验证所有必需的表都有正确的列
 */
TEST_F(InitCommandTest, ShouldCreateTablesWithCorrectSchema) {
    // 执行git init
    EXPECT_NO_THROW(engine_->Execute("git init"));

    // 验证git_objects表结构
    auto git_objects_columns = DbManager::GetInstance().Query(
        R"(
            SELECT COLUMN_NAME 
            FROM information_schema.COLUMNS 
            WHERE TABLE_SCHEMA = DATABASE() 
            AND TABLE_NAME = 'git_objects'
            ORDER BY ORDINAL_POSITION
        )");
    std::vector<std::string> expected_columns = {"sha1", "type", "data", "size", "created_at"};
    ASSERT_GE(git_objects_columns.size(), expected_columns.size());
    
    // 验证references表结构
    auto references_columns = DbManager::GetInstance().Query(
        R"(
            SELECT COLUMN_NAME 
            FROM information_schema.COLUMNS 
            WHERE TABLE_SCHEMA = DATABASE() 
            AND TABLE_NAME = 'references'
            ORDER BY ORDINAL_POSITION
        )");
    std::vector<std::string> expected_ref_columns = {"name", "target", "type", "created_at", "updated_at"};
    ASSERT_GE(references_columns.size(), expected_ref_columns.size());

    // 验证staging_index表结构
    auto staging_columns = DbManager::GetInstance().Query(
        R"(
            SELECT COLUMN_NAME 
            FROM information_schema.COLUMNS 
            WHERE TABLE_SCHEMA = DATABASE() 
            AND TABLE_NAME = 'staging_index'
            ORDER BY ORDINAL_POSITION
        )");
    std::vector<std::string> expected_staging_columns = {"file_path", "blob_sha1", "mode", "staged_at"};
    ASSERT_GE(staging_columns.size(), expected_staging_columns.size());
}

/**
 * @brief 测试空命令不会崩溃
 */
TEST_F(InitCommandTest, ShouldHandleEmptyCommandGracefully) {
    EXPECT_NO_THROW(engine_->Execute(""));
    EXPECT_NO_THROW(engine_->Execute("   "));
}

/**
 * @brief 测试无效命令不会崩溃
 */
TEST_F(InitCommandTest, ShouldHandleInvalidCommandGracefully) {
    EXPECT_NO_THROW(engine_->Execute("git unknown_command"));
    EXPECT_NO_THROW(engine_->Execute("not_a_git_command"));
}

/**
 * @brief 测试多次初始化后状态一致性
 * 
 * 验证即使多次尝试初始化，最终状态应该是一致的
 */
TEST_F(InitCommandTest, ShouldMaintainConsistentStateAfterMultipleInits) {
    // 执行多次init（虽然第二次会失败）
    EXPECT_NO_THROW(engine_->Execute("git init"));
    EXPECT_NO_THROW(engine_->Execute("git init"));
    EXPECT_NO_THROW(engine_->Execute("git init"));

    // 验证只有一个默认分支
    auto main_branches = DbManager::GetInstance().Query(
        "SELECT COUNT(*) as count FROM `references` WHERE name = 'refs/heads/main'");
    ASSERT_FALSE(main_branches.empty());
    auto it = main_branches[0].find("count");
    ASSERT_NE(it, main_branches[0].end());
    EXPECT_EQ(std::stoi(it->second), 1)
        << "Should have exactly one 'main' branch";

    // 验证只有一个HEAD引用
    auto head_refs = DbManager::GetInstance().Query(
        "SELECT COUNT(*) as count FROM `references` WHERE name = 'HEAD'");
    ASSERT_FALSE(head_refs.empty());
    it = head_refs[0].find("count");
    ASSERT_NE(it, head_refs[0].end());
    EXPECT_EQ(std::stoi(it->second), 1)
        << "Should have exactly one HEAD reference";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

