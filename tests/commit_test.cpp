#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <cctype>
#include "../include/commit.h"
#include <chrono>
#include <thread>


/**测试commit类构造的item */
TEST(CommitTest, CommitItemFormat) {
    GTEST_LOG_(INFO) << "测试仓库首个commit item";
    const std::string init_parameter_msg("init: test_msg");
    Commit commit_init(init_parameter_msg, "", {});
    const std::stringstream& init_result = commit_init.get_commit_item();
    GTEST_LOG_(INFO) << "commit_init: " << init_result.str();
    // 读commit的成员
    std::string init_father_ref, init_current_ref, init_current_timestamp, init_msg;
    std::stringstream init_ss(init_result.str());
    // 反序列化读取commit中的成员
    size_t init_father_ref_len, init_current_ref_len, init_current_timestamp_len, init_msg_len;
    init_ss >> init_father_ref_len;
    init_father_ref.resize(init_father_ref_len);
    init_ss.ignore(1);
    init_ss.read(&init_father_ref[0], init_father_ref_len);
    init_ss >> init_current_ref_len;
    init_current_ref.resize(init_current_ref_len);
    init_ss.ignore(1);
    init_ss.read(&init_current_ref[0], init_current_ref_len);
    init_ss >> init_current_timestamp_len;
    init_current_timestamp.resize(init_current_timestamp_len);
    init_ss.ignore(1);
    init_ss.read(&init_current_timestamp[0], init_current_timestamp_len);
    init_ss >> init_msg_len;
    init_msg.resize(init_msg_len);
    init_ss.ignore(1);
    init_ss.read(&init_msg[0], init_msg_len);
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
    Commit anyone_commit(anyone_parameter_msg, init_current_ref, {});
    const std::stringstream& anyone_result = anyone_commit.get_commit_item();
    std::stringstream anyone_ss(anyone_result.str());
    GTEST_LOG_(INFO) << "anyone_commit: " << anyone_result.str();
    std::string anyone_father_ref, anyone_current_ref, anyone_current_timestamp, anyone_msg;
    size_t anyone_father_ref_len, anyone_current_ref_len, anyone_current_timestamp_len, anyone_msg_len;
    anyone_ss >> anyone_father_ref_len;
    anyone_father_ref.resize(anyone_father_ref_len);
    anyone_ss.ignore(1);
    anyone_ss.read(&anyone_father_ref[0], anyone_father_ref_len);
    anyone_ss >> anyone_current_ref_len;
    anyone_current_ref.resize(anyone_current_ref_len);
    anyone_ss.ignore(1);
    anyone_ss.read(&anyone_current_ref[0], anyone_current_ref_len);
    anyone_ss >> anyone_current_timestamp_len;
    anyone_current_timestamp.resize(anyone_current_timestamp_len);
    anyone_ss.ignore(1);
    anyone_ss.read(&anyone_current_timestamp[0], anyone_current_timestamp_len);
    anyone_ss >> anyone_msg_len;
    anyone_msg.resize(anyone_msg_len);
    anyone_ss.ignore(1);
    anyone_ss.read(&anyone_msg[0], anyone_msg_len);
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
    Commit commit1("same commit", "", {});
    // 延时2s
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Commit commit2("same commit", "", {});
    const std::stringstream& result1 = commit1.get_commit_item();
    GTEST_LOG_(INFO) << "commit_init: " << result1.str();
    // 读commit的成员
    std::string father_ref1, current_ref1;
    std::stringstream ss1(result1.str());
    // 反序列化读取commit中的成员
    size_t father_ref_len1, current_ref_len1;
    ss1 >> father_ref_len1;
    father_ref1.resize(father_ref_len1);
    ss1.ignore(1);
    ss1.read(&father_ref1[0], father_ref_len1);
    ss1 >> current_ref_len1;
    current_ref1.resize(current_ref_len1);
    ss1.ignore(1);
    ss1.read(&current_ref1[0], current_ref_len1);

    const std::stringstream& result2 = commit2.get_commit_item();
    GTEST_LOG_(INFO) << "commit_init: " << result2.str();
    // 读commit的成员
    std::string father_ref2, current_ref2;
    std::stringstream ss2(result2.str());
    // 反序列化读取commit中的成员
    size_t father_ref_len2, current_ref_len2;
    ss2 >> father_ref_len2;
    father_ref2.resize(father_ref_len2);
    ss2.ignore(1);
    ss2.read(&father_ref2[0], father_ref_len2);
    ss2 >> current_ref_len2;
    current_ref2.resize(current_ref_len2);
    ss2.ignore(1);
    ss2.read(&current_ref2[0], current_ref_len2);
    EXPECT_NE(current_ref1, current_ref2);
}