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