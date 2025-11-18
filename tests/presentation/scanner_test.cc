#include "presentation/scanner.h"
#include <gtest/gtest.h>
#include <vector>

using namespace presentation;

// 这是一个辅助函数，用于比较两个 Token 向量是否相等。
// 它可以让我们的测试代码更简洁、易读。
void AssertTokensEqual(const std::vector<Token>& actual, const std::vector<Token>& expected) {
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i].type, expected[i].type) << "Token " << i << " type mismatch.";
        EXPECT_EQ(actual[i].lexeme, expected[i].lexeme) << "Token " << i << " lexeme mismatch.";
        EXPECT_EQ(actual[i].position, expected[i].position) << "Token " << i << " position mismatch.";
    }
}

TEST(ScannerTest, HandlesSimpleCommand) {
    Scanner scanner("git init");
    auto tokens = scanner.ScanTokens();
    AssertTokensEqual(tokens, {
        {TokenType::kIdentifier, "git", 0},
        {TokenType::kIdentifier, "init", 4},
        {TokenType::kEndOfFile, "", 8}
    });
}

TEST(ScannerTest, HandlesShortFlagAndQuotedValue) {
    Scanner scanner("commit -m \"Initial commit\"");
    auto tokens = scanner.ScanTokens();
    AssertTokensEqual(tokens, {
        {TokenType::kIdentifier, "commit", 0},
        {TokenType::kShortFlag, "-m", 7},
        {TokenType::kQuotedValue, "Initial commit", 10},
        {TokenType::kEndOfFile, "", 26}
    });
}

TEST(ScannerTest, HandlesLongFlagWithValueUsingEquals) {
    Scanner scanner("--message=\"hello world\"");
    auto tokens = scanner.ScanTokens();
    AssertTokensEqual(tokens, {
        {TokenType::kLongFlag, "--message", 0},
        {TokenType::kEquals, "=", 9},
        {TokenType::kQuotedValue, "hello world", 10},
        {TokenType::kEndOfFile, "", 23}
    });
}

TEST(ScannerTest, HandlesMultipleFilePathsAsValues) {
    Scanner scanner("add src/main.cc include/app.h");
    auto tokens = scanner.ScanTokens();
    AssertTokensEqual(tokens, {
        {TokenType::kIdentifier, "add", 0},
        {TokenType::kValue, "src/main.cc", 4},
        {TokenType::kValue, "include/app.h", 16},
        {TokenType::kEndOfFile, "", 29}
    });
}

TEST(ScannerTest, HandlesUnterminatedString) {
    Scanner scanner("commit -m \"hello");
    auto tokens = scanner.ScanTokens();
    AssertTokensEqual(tokens, {
        {TokenType::kIdentifier, "commit", 0},
        {TokenType::kShortFlag, "-m", 7},
        {TokenType::kUnknown, "hello", 10},
        {TokenType::kEndOfFile, "", 16}
    });
}

TEST(ScannerTest, HandlesComplexCommand) {
    Scanner scanner("git commit --amend -m \"new message\"");
    auto tokens = scanner.ScanTokens();
    AssertTokensEqual(tokens, {
        {TokenType::kIdentifier, "git", 0},
        {TokenType::kIdentifier, "commit", 4},
        {TokenType::kLongFlag, "--amend", 11},
        {TokenType::kShortFlag, "-m", 19},
        {TokenType::kQuotedValue, "new message", 22},
        {TokenType::kEndOfFile, "", 35}
    });
}