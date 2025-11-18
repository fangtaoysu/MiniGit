#include <gtest/gtest.h>

#include "infrastructure/logging/logger.h"


// 实现对INFO、Warning、Error级别的日志记录
TEST(LoggerTest, InitializeAndLog) {
    // Initialize the logger using the config file
    std::filesystem::path log_config_path = PathUtils::GetProjectRoot() / "config" / "log4cplus.properties";
    InitImLogger(log_config_path.string());

    // Log messages at different levels to ensure it doesn't crash
    // and to visually inspect the output when running tests.
    LOG_INFO("This is an info message from the test.");
    LOG_WARN("This is a warning message from the test.");
    LOG_ERROR("This is an error message from the test.");

    // The test passes if the logger initializes and logs without crashing.
    SUCCEED();
}