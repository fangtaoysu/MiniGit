#include "presentation/lexer.h"

#include <gtest/gtest.h>

#include "infrastructure/logging/logger.h"
#include "presentation/model.h"

using namespace minigit::infrastructure::logging;
using namespace minigit::presentation;
using namespace minigit::shared;

// Helper function to compare two LexicalResult objects for equality.
void AssertResultEqual(const CommandContext& actual,
                       const CommandContext& expected) {
    EXPECT_EQ(actual.cmd, expected.cmd);
    EXPECT_EQ(actual.opts, expected.opts);
    EXPECT_EQ(actual.args, expected.args);
    EXPECT_EQ(actual.file_paths, expected.file_paths);
}

TEST(LexerTest, HandlesSimpleInitCommand) {
    Lexer lexer;
    std::string command_str = "git init";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;
    expected.cmd = "init";

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesCommitWithShortOptionAndQuotedMessage) {
    Lexer lexer;
    std::string command_str = "git commit -m \"repo init: \"apx\" is ok\"";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;
    expected.cmd = "commit";
    expected.opts = {"-m"};
    expected.args = {"repo init: \"apx\" is ok"};

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesNoGit) {
    Lexer lexer;
    std::string command_str = "add src/main.cc include/app.h";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesAddWithMultipleFiles) {
    Lexer lexer;
    std::string command_str = "git add src/main.cc include/app.h";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;
    expected.cmd = "add";
    expected.file_paths = {"src/main.cc", "include/app.h"};

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesComplexCommitCommand) {
    Lexer lexer;
    std::string command_str = "git commit --amend -m \"new message\"";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;
    expected.cmd = "commit";
    expected.opts = {"--amend", "-m"};
    expected.args = {"new message"};

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesCommandWithoutGitPrefix) {
    Lexer lexer;
    std::string command_str = "git status";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;
    expected.cmd = "status";

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesEmptyCommand) {
    Lexer lexer;
    std::string command_str = "";
    CommandContext actual = lexer.LexicalAnalyze(command_str);

    CommandContext expected;  // All fields should be empty

    AssertResultEqual(actual, expected);
}

int main(int argc, char** argv) {
    // Initialize the logger
    const std::filesystem::path project_root = GetProjectRoot();
    std::filesystem::path log_config_path =
        project_root / "config" / "log4cplus.properties";
    InitImLogger(log_config_path.string());


    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}