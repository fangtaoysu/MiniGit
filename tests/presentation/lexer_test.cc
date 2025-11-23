#include "presentation/lexer.h"

#include <gtest/gtest.h>

#include "infrastructure/config/app_config.h"
#include "infrastructure/logging/logger.h"
#include "presentation/model.h"

using namespace minigit::infrastructure::config;
using namespace minigit::infrastructure::logging;
using namespace minigit::presentation;
using namespace minigit::shared;

// Helper function to compare two LexicalResult objects for equality.
void AssertResultEqual(const LexicalResult& actual,
                       const LexicalResult& expected) {
    EXPECT_EQ(actual.command, expected.command);
    EXPECT_EQ(actual.option, expected.option);
    EXPECT_EQ(actual.argument, expected.argument);
    EXPECT_EQ(actual.file_path, expected.file_path);
}

TEST(LexerTest, HandlesSimpleInitCommand) {
    Lexer lexer;
    std::string command_str = "git init";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;
    expected.command = "init";

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesCommitWithShortOptionAndQuotedMessage) {
    Lexer lexer;
    std::string command_str = "git commit -m \"repo init: \"apx\" is ok\"";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;
    expected.command = "commit";
    expected.option = {"-m"};
    expected.argument = {"repo init: \"apx\" is ok"};

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesNoGit) {
    Lexer lexer;
    std::string command_str = "add src/main.cc include/app.h";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesAddWithMultipleFiles) {
    Lexer lexer;
    std::string command_str = "git add src/main.cc include/app.h";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;
    expected.command = "add";
    expected.file_path = {"src/main.cc", "include/app.h"};

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesComplexCommitCommand) {
    Lexer lexer;
    std::string command_str = "git commit --amend -m \"new message\"";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;
    expected.command = "commit";
    expected.option = {"--amend", "-m"};
    expected.argument = {"new message"};

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesCommandWithoutGitPrefix) {
    Lexer lexer;
    std::string command_str = "git status";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;
    expected.command = "status";

    AssertResultEqual(actual, expected);
}

TEST(LexerTest, HandlesEmptyCommand) {
    Lexer lexer;
    std::string command_str = "";
    LexicalResult actual = lexer.LexicalAnalyze(command_str);

    LexicalResult expected;  // All fields should be empty

    AssertResultEqual(actual, expected);
}

int main(int argc, char** argv) {
    // Initialize the logger
    const std::filesystem::path project_root = GetProjectRoot();
    std::filesystem::path log_config_path =
        project_root / "config" / "log4cplus.properties";

    InitImLogger(log_config_path.string());

    std::filesystem::path app_config_path =
        project_root / "config" / "config.json";
    if (!AppConfig::GetInstance().LoadConfig(app_config_path)) {
        LOG_FATAL("Failed to load application config. Exiting.");
        return 1;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}