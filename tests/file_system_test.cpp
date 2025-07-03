#include <gtest/gtest.h>
#include "../include/file_system.h"
#include <vector>


TEST(FileSystemTest, IsTextFile) {
    std::vector<std::string> true_file_paths = {
        "../README.md", "../src/commit.cpp", "../include/commit.h"
    };
    // 文件不存在、可执行文件和文件夹
    std::vector<std::string> false_file_paths = {
        "../tests/test", "../build/test_commit/test_commit", "../src"
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