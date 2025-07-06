#include <gtest/gtest.h>
#include <string>
#include "../include/utils.h"

TEST(UtilsTest, SameInputGeneratesSameHash) {
    std::string input = "Hello, world!";
    std::string hash1 = Utils::get_hash(input);
    std::string hash2 = Utils::get_hash(input);
    
    EXPECT_EQ(hash1, hash2) << "相同输入应生成相同的哈希值";
}

TEST(UtilsTest, DifferentInputGeneratesDifferentHash) {
    std::string input1 = "Hello, world!";
    std::string input2 = "Hello, GPT!";
    std::string hash1 = Utils::get_hash(input1);
    std::string hash2 = Utils::get_hash(input2);

    EXPECT_NE(hash1, hash2) << "不同输入应生成不同的哈希值";
}

TEST(UtilsTest, EmptyStringHasFixedHash) {
    std::string empty_input = "";
    std::string hash = Utils::get_hash(empty_input);
    
    // SHA1("") 的标准哈希值
    std::string expected = "da39a3ee5e6b4b0d3255bfef95601890afd80709";
    EXPECT_EQ(hash, expected) << "空字符串的 SHA1 应为固定值";
}