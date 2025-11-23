#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "infrastructure/config/app_config.h"

// Test fixture for AppConfig tests to handle temporary file creation and
// cleanup.
class AppConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a unique temporary file path for each test.
        temp_config_path_ =
            std::filesystem::temp_directory_path() / "test_config.json";
    }

    void TearDown() override {
        // Clean up the temporary file after each test.
        std::filesystem::remove(temp_config_path_);
    }

    // Helper to write content to the temporary config file.
    void WriteConfigFile(const std::string& content) {
        std::ofstream file(temp_config_path_);
        file << content;
        file.close();
    }

    std::filesystem::path temp_config_path_;
};

TEST_F(AppConfigTest, LoadsFullConfigCorrectly) {
    const std::string full_config_json = R"(
    {
        "mysql": {
            "enable": true,
            "host": "db.example.com",
            "port": 3307,
            "user": "test_user",
            "password": "test_pass",
            "db_name": "test_db",
            "pool_size": 16
        },
        "thread_pool": {
            "size": 8
        },
        "logging": {
            "level": "debug"
        }
    }
    )";
    WriteConfigFile(full_config_json);

    AppConfig config;
    ASSERT_TRUE(config.LoadConfig(temp_config_path_));

    const auto& db = config.GetMySqlSettings();
    EXPECT_TRUE(db.enable);
    EXPECT_EQ(db.host, "db.example.com");
    EXPECT_EQ(db.port, 3307);
    EXPECT_EQ(db.user, "test_user");
    EXPECT_EQ(db.password, "test_pass");
    EXPECT_EQ(db.db_name, "test_db");
    EXPECT_EQ(db.pool_size, 16);

    const auto& pool = config.GetThreadPoolSettings();
    EXPECT_EQ(pool.size, 8);

    const auto& log = config.GetLoggingSettings();
    EXPECT_EQ(log.level, "debug");
}

TEST_F(AppConfigTest, HandlesPartialConfigWithDefaults) {
    const std::string partial_config_json = R"(
    {
        "mysql": {
            "enable": true,
            "user": "partial_user",
            "db_name": "partial_db"
        }
    }
    )";
    WriteConfigFile(partial_config_json);

    AppConfig config;
    ASSERT_TRUE(config.LoadConfig(temp_config_path_));

    const auto& db = config.GetMySqlSettings();
    EXPECT_TRUE(db.enable);
    EXPECT_EQ(db.host, "127.0.0.1");
    EXPECT_EQ(db.port, 3306);
    EXPECT_EQ(db.user, "partial_user");
    EXPECT_EQ(db.password, "");
    EXPECT_EQ(db.db_name, "partial_db");
    EXPECT_EQ(db.pool_size, 4);

    const auto& pool = config.GetThreadPoolSettings();
    EXPECT_EQ(pool.size, 4);

    const auto& log = config.GetLoggingSettings();
    EXPECT_EQ(log.level, "info");
}

TEST_F(AppConfigTest, HandlesMissingFile) {
    AppConfig config;
    // Attempt to load a non-existent file.
    EXPECT_FALSE(config.LoadConfig("non_existent_file.json"));
}

TEST_F(AppConfigTest, HandlesInvalidJson) {
    const std::string invalid_json = R"(
    {
        "mysql": {
            "enable": true,
    }
    )";  // Malformed JSON
    WriteConfigFile(invalid_json);

    AppConfig config;
    EXPECT_FALSE(config.LoadConfig(temp_config_path_));
}