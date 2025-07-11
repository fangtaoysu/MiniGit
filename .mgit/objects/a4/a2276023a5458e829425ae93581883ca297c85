#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include "../include/file_system.h"
#include <vector>


/** 测试is_text_file函数功能 */
TEST(FileSystemTest, IsTextFile) {
    std::vector<std::string> true_file_paths = {
        "../README.md", "../src/commit.cpp", "../include/commit.h"
    };
    // 文件不存在、可执行文件和文件夹
    std::vector<std::string> false_file_paths = {
        "../tests/test", "../build/test_commit/test_commit", "../src", "../tmp/epoch_5.png"
    };
    for (auto & file_path : true_file_paths) {
        GTEST_LOG_(INFO) << file_path << FileSystem::is_text_file(file_path);
        EXPECT_TRUE(FileSystem::is_text_file(file_path));
    }
    for (auto & file_path : false_file_paths) {
        GTEST_LOG_(INFO) << file_path << FileSystem::is_text_file(file_path);
        EXPECT_FALSE(FileSystem::is_text_file(file_path));
    }
}

/** 测试append_file_content 函数功能 */
TEST(FileSystemTest, AppendFileContent) {
    const std::string file_path = "../tmp/.mgit/logs/HEAD";
    namespace fs = std::filesystem;

    if (fs::exists(file_path)) {
        fs::remove(file_path);
    }
    const std::string append_content("be1c918f68b3b7b1ded342e39b96e90def1bb8ce 23c7d5bd49050851d4fb54ab42920dcb9d3c104f fangtaoysu <18394604239@163.com> 1751558735 +0800     commit: judge is text file or not\n");
    FileSystem::append_file_content(file_path, append_content);
    std::ifstream ifs(file_path);
    ASSERT_TRUE(ifs.is_open());

    std::string file_content((std::istreambuf_iterator<char>(ifs)),
                             std::istreambuf_iterator<char>());
    ifs.close();

    EXPECT_NE(file_content.find(append_content), std::string::npos);

    // 再次追加，验证追加效果
    ASSERT_NO_THROW(FileSystem::append_file_content(file_path, append_content));

    std::ifstream ifs2(file_path);
    std::string file_content2((std::istreambuf_iterator<char>(ifs2)),
                              std::istreambuf_iterator<char>());
    ifs2.close();

    int count = 0;
    size_t pos = 0;
    while ((pos = file_content2.find(append_content, pos)) != std::string::npos) {
        ++count;
        pos += append_content.size();
    }
    EXPECT_GE(count, 2);
    if (fs::exists(file_path)) {
        fs::remove(file_path);
    }
}

/**测试 get_all_files 函数 */
TEST(FileSystemTest, GetAllFiles) {
    // 用项目路径测试
    const std::string project_path("/data/fangtao/MiniGit/tmp");
    std::vector<fs::path> files = FileSystem::get_all_files(project_path);
    
    // 只能通过肉眼验证结果
    ASSERT_FALSE(files.empty());
    for (const auto& file : files) {
        GTEST_LOG_(INFO) << "Found file: " << file << std::endl;
    }    
}

/** 测试 read_file 函数 */
TEST(FileSystemTest, ReadFile) {
    // 准备测试文件
    const fs::path test_dir = fs::current_path() / "test_temp";
    fs::create_directories(test_dir);
    const fs::path test_file = test_dir / "test_file.txt";
    const std::string test_content = "Hello MiniGit!\nThis is a test file.\n";
    
    // 写入测试文件
    std::ofstream(test_file) << test_content;

    // ---------------------------
    // 测试1：正常读取文件
    // ---------------------------
    {
        std::stringstream contents = FileSystem::read_file(test_file);
        
        // 验证内容匹配
        std::stringstream expected;
        expected << test_content;
        EXPECT_EQ(contents.str(), expected.str());
        
        // 验证读取后流状态正常
        EXPECT_FALSE(contents.fail());
        EXPECT_TRUE(contents.good());
    }

    // ---------------------------
    // 测试2：读取不存在的文件
    // ---------------------------
    {
        fs::path not_exist_file = test_dir / "not_exist.txt";
        EXPECT_THROW({
            FileSystem::read_file(not_exist_file);
        }, std::runtime_error);
    }

    // ---------------------------
    // 测试3：读取目录（非法操作）
    // ---------------------------
    {
        EXPECT_THROW({
            FileSystem::read_file(test_dir);
        }, std::runtime_error);
    }

    // ---------------------------
    // 测试4：读取二进制文件
    // ---------------------------
    {
        const fs::path binary_file = test_dir / "binary.data";
        const std::vector<uint8_t> binary_data{0x00, 0x01, 0x02, 0xFF};
        
        // 写入二进制数据
        std::ofstream(binary_file, std::ios::binary)
            .write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        
        // 读取并验证
        std::stringstream contents = FileSystem::read_file(binary_file);
        std::string read_data = contents.str();
        
        EXPECT_EQ(read_data.size(), binary_data.size());
        EXPECT_EQ(read_data[0], '\0');
        EXPECT_EQ(static_cast<uint8_t>(read_data[3]), 0xFF);
    }

    // 清理测试文件
    fs::remove_all(test_dir);
}


/** 测试copy_file_to 函数功能 */
TEST(FileSystemTest, BasicCopy) {
    // 准备测试文件
    std::string src = "test_src.txt";
    std::ofstream(src) << "test content";

    // 设置目标路径
    fs::path dst("/data/fangtao/MiniGit/tmp/.mgit/object/35/99cdd13328b2b21e29a95405c301b9313b7c346");

    // 测试复制函数
    EXPECT_TRUE(FileSystem::copy_file_to(src, dst));
    EXPECT_TRUE(fs::exists(dst));

    // 验证内容
    std::ifstream copy_file_ifs(dst);
    std::string content((std::istreambuf_iterator<char>(copy_file_ifs)),
                        std::istreambuf_iterator<char>());
    EXPECT_EQ(content, "test content");

    // 清理
    fs::remove(src);
    fs::remove(dst);
}

TEST(FileSystemTest, GetFileSize_ValidFile) {
    fs::path test_path("/data/fangtao/MiniGit/tmp/tests/commit_test.cpp");
    GTEST_LOG_(INFO) << "Exists: " << fs::exists(test_path) << "\n";
    int64_t size = FileSystem::get_file_size(test_path);
    EXPECT_GT(size, 0);  // 文件大小应大于0
}

TEST(FileSystemTest, GetFileSize_InvalidFile) {
    fs::path non_existent = "non_existent_file.txt";
    int64_t size = FileSystem::get_file_size(non_existent);
    EXPECT_EQ(size, -1);  // 不存在的文件应返回 -1
}

TEST(FileSystemTest, GetFileTimestamp_ValidFile) {
    fs::path test_path("/data/fangtao/MiniGit/tmp/tests/commit_test.cpp");
    GTEST_LOG_(INFO) << "Exists: " << fs::exists(test_path) << "\n";
    int64_t timestamp = FileSystem::get_file_timestamp(test_path);
    EXPECT_GT(timestamp, 0);  // 应返回有效时间戳
    EXPECT_LE(std::to_string(timestamp).length(), 10);  // 返回 10 位以内时间戳
}

TEST(FileSystemTest, GetFileTimestamp_InvalidFile) {
    fs::path non_existent = "non_existent_file.txt";
    int64_t timestamp = FileSystem::get_file_timestamp(non_existent);
    EXPECT_EQ(timestamp, -1);  // 不存在的文件应返回 -1
}
