#include "../include/commit.h"
#include <gtest/gtest.h>
#include <cctype>
#include <chrono>
#include <thread>



/**测试commit类构造的item */
TEST(CommitTest, CommitObjectFormat) {
    GTEST_LOG_(INFO) << "测试仓库首个commit item";
    const std::string init_parameter_msg("init: test_msg");
    Commit commit_init(init_parameter_msg, "");
    const CommitObject& init_result = commit_init.get_commit_object();
    // 读commit的成员
    std::string init_father_ref, init_current_ref, init_current_timestamp, init_msg;
    init_father_ref = init_result.father_ref;
    init_current_ref = init_result.current_ref;
    init_current_timestamp = init_result.current_timestamp;
    init_msg = init_result.msg;
    // 逐个验证
    EXPECT_EQ(init_father_ref, std::string(40, '0'));
    EXPECT_EQ(init_current_ref.length(), 40);
    for (char c : init_current_ref) {
        EXPECT_TRUE(isdigit(c) || islower(c));
    }
    EXPECT_EQ(init_current_timestamp.length(), 10);
    for (char c : init_current_timestamp) {
        EXPECT_TRUE(isdigit(c));
    }
    EXPECT_EQ(init_msg, init_parameter_msg);

    GTEST_LOG_(INFO) << "测试仓库后续commit item";
    const std::string anyone_parameter_msg("feat:new message");
    Commit anyone_commit(anyone_parameter_msg, init_current_ref);
    const CommitObject& anyone_result = anyone_commit.get_commit_object();
    std::string anyone_father_ref, anyone_current_ref, anyone_current_timestamp, anyone_msg;
    anyone_father_ref = anyone_result.father_ref;
    anyone_current_ref = anyone_result.current_ref;
    anyone_current_timestamp = anyone_result.current_timestamp;
    anyone_msg = anyone_result.msg;
    // 开始验证commit对象
    EXPECT_EQ(anyone_father_ref, init_current_ref);
    EXPECT_NE(anyone_father_ref, std::string('0', 40));
    EXPECT_EQ(anyone_current_ref.length(), 40);
    for (char c : anyone_current_ref) {
        EXPECT_TRUE(isdigit(c) || islower(c));
    }
    EXPECT_EQ(anyone_current_timestamp.length(), 10);
    for (char c : anyone_current_timestamp) {
        EXPECT_TRUE(isdigit(c));
    }
    EXPECT_EQ(anyone_msg, anyone_parameter_msg);
}

/**测试commit类生成哈希的唯一性 */
TEST(CommitTest, CommitHashUniqueness) {
    Commit commit1("same commit", "");
    // 延时2s
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Commit commit2("same commit", "");
    const CommitObject& result1 = commit1.get_commit_object();
    const CommitObject& result2 = commit2.get_commit_object();
    
    const std::string& current_ref1 = result1.current_ref;
    const std::string& current_ref2 = result2.current_ref;
    EXPECT_NE(current_ref1, current_ref2);
}