#include <gtest/gtest.h>
#include "infrastructure/config/config_loader.h"

TEST(ConfigLoaderTest, LoadConfig) {
    minigit::infrastructure::config::ConfigLoader loader;
    loader.LoadConfig();

    const auto& config = loader.GetConfig();
    EXPECT_EQ(config.logging.level, "info");
    EXPECT_TRUE(config.mysql.enable);
    EXPECT_EQ(config.mysql.host, "127.0.0.1");
    EXPECT_EQ(config.mysql.port, 3306);
    EXPECT_EQ(config.mysql.user, "root");
    EXPECT_EQ(config.mysql.password, "root");
    EXPECT_EQ(config.mysql.database, "mini_git");
    EXPECT_EQ(config.thread_pool.size, 8);
}