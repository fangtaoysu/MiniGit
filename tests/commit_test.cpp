#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include "../include/commit.h"


TEST(CommitTest, TimestampFormat) {
    Commit commit("init: test_msg", "", {});
    std::string timestamp = commit.get_current_timestamp();
    GTEST_LOG_(INFO) << "Generated timestamp: " << timestamp;
    
    EXPECT_EQ(timestamp.length(), 10);
    for (char c : timestamp) {
        EXPECT_TRUE(isdigit(c));
    }
}