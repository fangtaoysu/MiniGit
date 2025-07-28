#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <string>
#include "../include/utils.h"
#include <fstream>





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

/** 测试tokenize分词结果 */
TEST(UtilsTest, TokenizeResult) {
    struct TestCase {
        std::string input;
        std::vector<std::string> expected;
    };
    
    std::vector<TestCase> tests = {
        {
            "git add .", 
            {"git", "add", "."}
        },
        {
            "git add file1 file2", 
            {"git", "add", "file1", "file2"}
        },
        {
            "git commit -m \"fix: display error\"", 
            {"git", "commit", "-m", "fix: display error"}
        },
        {
            "git commit -m \"feat: new reuslt\n - complish features of add and commit.\n - complish these features test. \"",
            {"git", "commit", "-m", "feat: new reuslt\n - complish features of add and commit.\n - complish these features test. "}
        },
        {
            "  git  add ./src/commit.cpp ./include/commit.h  ", 
            {"git", "add", "./src/commit.cpp", "./include/commit.h"}
        },
        {
            "    git     commit    -m \"   git    init    \"", 
            {"git", "commit", "-m", "   git    init    "}
        }
    };

    for (size_t i = 0; i < tests.size(); ++i) {
        const auto& test = tests[i];
        SCOPED_TRACE("Test case #" + std::to_string(i) + ": " + test.input);
        
        std::vector<std::string> results = Utils::tokenize(test.input);
        EXPECT_EQ(results, test.expected);
    }

    EXPECT_EQ('\"', '"');
}

class PathUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建临时测试目录结构
        fs::create_directories(base_dir / "a/b/c");
        fs::create_directories(base_dir / "x/y");
        std::ofstream(base_dir / "a/file.txt").put('x');
        
        // 创建符号链接（Unix 示例）
        if (!fs::exists(base_dir / "a/link")) {
            fs::create_symlink("../x", base_dir / "a/link");
        }
    }

    void TearDown() override {
        fs::remove_all(base_dir);  // 清理测试目录
    }

    const fs::path base_dir = fs::temp_directory_path() / "test_path_utils";
};

// 测试正常子路径
TEST_F(PathUtilsTest, DirectSubpath) {
    EXPECT_TRUE(Utils::is_subpath(base_dir, base_dir / "a/b/c"));
    EXPECT_TRUE(Utils::is_subpath(base_dir / "a", base_dir / "a/file.txt"));
}

// 测试相对路径和父级引用
TEST_F(PathUtilsTest, RelativePaths) {
    EXPECT_TRUE(Utils::is_subpath(base_dir / "a", base_dir / "./a/b"));
    EXPECT_FALSE(Utils::is_subpath(base_dir / "a/b", base_dir / "a/../x"));
}

// 测试不存在的路径
TEST_F(PathUtilsTest, NonExistentPaths) {
    EXPECT_FALSE(Utils::is_subpath(base_dir, base_dir / "ghost"));
    EXPECT_FALSE(Utils::is_subpath(base_dir / "ghost", base_dir / "a"));
}

// 测试完全无关的路径
TEST_F(PathUtilsTest, UnrelatedPaths) {
    EXPECT_FALSE(Utils::is_subpath("/usr", "/tmp"));
    EXPECT_FALSE(Utils::is_subpath(base_dir, "/home"));
}

// 测试自身是否是自己的子路径
TEST_F(PathUtilsTest, SelfIsSubpath) {
    EXPECT_TRUE(Utils::is_subpath(base_dir, base_dir));  // 规范化后相同
}