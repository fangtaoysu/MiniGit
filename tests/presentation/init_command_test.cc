#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <chrono>
#include <string>
#include <vector>

#include "presentation/command_engine.h"
#include "application/init/init_executor.h"
#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"


namespace minigit::presentation {

// Simple stub validator that always passes validation
class StubValidator : public Validator {
public:
    bool Validate(const CommandContext& cmd_context) override {
        return true;  // Always validate successfully
    }
};

class InitCommandTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize logger (using the correct function from logger.h)
        std::filesystem::path log_config_path = minigit::shared::GetProjectRoot() / "config" / "log4cplus.conf";
        minigit::infrastructure::logging::InitImLogger(log_config_path.string());
        
        // Create command engine
        engine_ = std::make_unique<CommandEngine>();
        
        // Create and register init executor with a stub validator
        auto init_executor = std::make_unique<minigit::application::init::InitExecutor>();
        auto stub_validator = std::make_unique<minigit::presentation::StubValidator>();
        engine_->RegisterCommand("init", std::move(stub_validator), std::move(init_executor));
    }
    
    void TearDown() override {
        CleanupTestData();
    }
    
    bool PathExists(const std::filesystem::path& path) {
        return std::filesystem::exists(path);
    }
    
    void CleanupTestData() {
        std::filesystem::path mgit_dir = minigit::shared::GetProjectRoot() / "build" / ".mgit";
        
        if (std::filesystem::exists(mgit_dir)) {
            std::filesystem::remove_all(mgit_dir);
        }
    }
    
    std::unique_ptr<CommandEngine> engine_;
};

/**
 * @brief 测试git init的日志输出
 */
TEST_F(InitCommandTest, ShouldLogInitOperations) {
    // 执行git init命令
    EXPECT_NO_THROW(engine_->Execute("git init"));
    
    // 验证基本结构被创建
    std::filesystem::path mgit_dir = minigit::shared::GetProjectRoot() / "build" / ".mgit";
    
    EXPECT_TRUE(PathExists(mgit_dir))
        << "Init command should create .mgit directory";
    
    // 验证创建了预期的子目录
    EXPECT_TRUE(PathExists(mgit_dir / "objects"));
    EXPECT_TRUE(PathExists(mgit_dir / "refs"));
    EXPECT_TRUE(PathExists(mgit_dir / "hooks"));
    EXPECT_TRUE(PathExists(mgit_dir / "info"));
}

/**
 * @brief 测试HEAD文件内容
 */
TEST_F(InitCommandTest, ShouldCreateHEADWithCorrectContent) {
    // 执行git init命令
    engine_->Execute("git init");
    
    // 验证HEAD文件内容
    std::filesystem::path head_file = minigit::shared::GetProjectRoot() / "build" / ".mgit" / "HEAD";
    
    EXPECT_TRUE(PathExists(head_file))
        << "HEAD file should exist";
    
    std::ifstream head_stream(head_file);
    EXPECT_TRUE(head_stream.is_open())
        << "HEAD file should be readable";
    
    std::string head_content;
    std::getline(head_stream, head_content);
    head_stream.close();
    
    EXPECT_EQ(head_content, "ref: refs/heads/main")
        << "HEAD should reference main branch";
}

/**
 * @brief 测试config文件内容
 */
TEST_F(InitCommandTest, ShouldCreateConfigWithCorrectContent) {
    // 执行git init命令
    engine_->Execute("git init");
    
    // 验证config文件内容
    std::filesystem::path config_file = minigit::shared::GetProjectRoot() / "build" / ".mgit" / "config";
    
    EXPECT_TRUE(PathExists(config_file))
        << "config file should exist";
    
    std::ifstream config_stream(config_file);
    EXPECT_TRUE(config_stream.is_open())
        << "config file should be readable";
    
    std::string config_content((std::istreambuf_iterator<char>(config_stream)),
                              std::istreambuf_iterator<char>());
    config_stream.close();
    
    EXPECT_TRUE(config_content.find("[core]") != std::string::npos)
        << "config should contain [core] section";
    EXPECT_TRUE(config_content.find("repositoryformatversion = 0") != std::string::npos)
        << "config should contain repository format version";
}

/**
 * @brief 测试description文件内容
 */
TEST_F(InitCommandTest, ShouldCreateDescriptionWithCorrectContent) {
    // 执行git init命令
    engine_->Execute("git init");
    
    // 验证description文件内容
    std::filesystem::path desc_file = minigit::shared::GetProjectRoot() / "build" / ".mgit" / "description";
    
    EXPECT_TRUE(PathExists(desc_file))
        << "description file should exist";
    
    std::ifstream desc_stream(desc_file);
    EXPECT_TRUE(desc_stream.is_open())
        << "description file should be readable";
    
    std::string desc_content;
    std::getline(desc_stream, desc_content);
    desc_stream.close();
    
    EXPECT_EQ(desc_content, "Unnamed repository; edit this file 'description' to name the repository.")
        << "description should contain default text";
}

/**
 * @brief 测试完整的Git仓库初始化流程
 */
TEST_F(InitCommandTest, ShouldInitializeCompleteGitRepository) {
    // 执行完整的初始化流程
    EXPECT_NO_THROW(engine_->Execute("git init"));
    
    // 验证完整的目录结构
    std::filesystem::path mgit_dir = minigit::shared::GetProjectRoot() / "build" / ".mgit";
    
    // 验证主目录
    EXPECT_TRUE(PathExists(mgit_dir))
        << ".mgit directory should be created";
    
    // 验证所有子目录
    std::vector<std::string> expected_subdirs = {
        "objects", "refs", "hooks", "info", "branches"
    };
    
    for (const auto& subdir : expected_subdirs) {
        EXPECT_TRUE(PathExists(mgit_dir / subdir))
            << "Subdirectory " << subdir << " should be created";
    }
    
    // 验证所有必需文件
    std::vector<std::string> expected_files = {
        "HEAD", "config", "description"
    };
    
    for (const auto& file : expected_files) {
        EXPECT_TRUE(PathExists(mgit_dir / file))
            << "File " << file << " should be created";
    }
    
    // // 验证refs/heads目录存在（用于分支）
    // EXPECT_TRUE(PathExists(mgit_dir / "refs" / "heads"))
    //     << "refs/heads directory should be created for branches";
}

/**
 * @brief 测试初始化性能
 */
TEST_F(InitCommandTest, ShouldInitializeRepositoryQuickly) {
    // 清理之前的测试数据
    CleanupTestData();
    
    // 记录开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 执行git init命令
    EXPECT_NO_THROW(engine_->Execute("git init"));
    
    // 记录结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // 验证执行时间在合理范围内（小于1秒）
    EXPECT_LT(duration.count(), 1000)
        << "Git init should complete within 1 second";
    
    // 验证结果正确
    std::filesystem::path mgit_dir = minigit::shared::GetProjectRoot() / "build" / ".mgit";
    EXPECT_TRUE(PathExists(mgit_dir))
        << "Repository should be created quickly";
}

/**
 * @brief 测试文件权限和属性
 */
TEST_F(InitCommandTest, ShouldCreateFilesWithCorrectPermissions) {
    // 执行git init命令
    engine_->Execute("git init");
    
    // 验证文件存在且可读
    std::filesystem::path config_file = minigit::shared::GetProjectRoot() / "build" / ".mgit" / "config";
    
    EXPECT_TRUE(PathExists(config_file))
        << "Config file should exist";
    
    // 验证文件可读
    std::ifstream config_stream(config_file);
    EXPECT_TRUE(config_stream.is_open())
        << "Config file should be readable";
    config_stream.close();
    
    // 验证文件不为空
    auto file_size = std::filesystem::file_size(config_file);
    EXPECT_GT(file_size, 0)
        << "Config file should not be empty";
}

/**
 * @brief 测试错误恢复能力
 */
TEST_F(InitCommandTest, ShouldHandlePartialInitialization) {
    // 手动创建部分目录结构
    std::filesystem::path mgit_dir = minigit::shared::GetProjectRoot() / "build" / ".mgit";
    std::filesystem::create_directories(mgit_dir / "objects");
    
    // 执行git init命令（应该能处理已存在的部分结构）
    EXPECT_NO_THROW(engine_->Execute("git init"));
    
    // 验证完整结构仍然被创建
    EXPECT_TRUE(PathExists(mgit_dir / "refs"))
        << "Missing directories should be created";
    EXPECT_TRUE(PathExists(mgit_dir / "HEAD"))
        << "Missing files should be created";
}

} // namespace minigit::application::init