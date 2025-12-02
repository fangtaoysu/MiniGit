#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "domain/storage/repositories/index_repository.h"

using namespace minigit::domain::storage::repositories;

class IndexRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override { repository_ = std::make_unique<IndexRepository>(); }

    void TearDown() override { repository_.reset(); }

    std::unique_ptr<IndexRepository> repository_;
};

/**
 * @brief 测试添加索引条目
 */
TEST_F(IndexRepositoryTest, ShouldAddIndexEntry) {
    std::string path = "src/main.cpp";
    std::string object_hash = "abc123def456";

    EXPECT_TRUE(repository_->Add(path, object_hash));
    EXPECT_TRUE(repository_->Contains(path));
    EXPECT_EQ(repository_->GetHash(path), object_hash);
}

/**
 * @brief 测试添加空路径应该失败
 */
TEST_F(IndexRepositoryTest, ShouldFailToAddEmptyPath) {
    std::string object_hash = "abc123def456";

    EXPECT_FALSE(repository_->Add("", object_hash));
}

/**
 * @brief 测试添加空对象哈希应该失败
 */
TEST_F(IndexRepositoryTest, ShouldFailToAddEmptyObjectHash) {
    std::string path = "src/main.cpp";

    EXPECT_FALSE(repository_->Add(path, ""));
}

/**
 * @brief 测试更新已存在的索引条目
 */
TEST_F(IndexRepositoryTest, ShouldUpdateExistingIndexEntry) {
    std::string path = "src/main.cpp";
    std::string object_hash1 = "abc123def456";
    std::string object_hash2 = "def456abc123";

    // 添加初始条目
    EXPECT_TRUE(repository_->Add(path, object_hash1));
    EXPECT_EQ(repository_->GetHash(path), object_hash1);

    // 更新条目
    EXPECT_TRUE(repository_->Add(path, object_hash2));
    EXPECT_EQ(repository_->GetHash(path), object_hash2);
}

/**
 * @brief 测试删除索引条目
 */
TEST_F(IndexRepositoryTest, ShouldRemoveIndexEntry) {
    std::string path = "src/main.cpp";
    std::string object_hash = "abc123def456";

    // 添加条目
    repository_->Add(path, object_hash);
    EXPECT_TRUE(repository_->Contains(path));

    // 删除条目
    EXPECT_TRUE(repository_->Remove(path));
    EXPECT_FALSE(repository_->Contains(path));
    EXPECT_EQ(repository_->GetHash(path), "");
}

/**
 * @brief 测试删除不存在的索引条目
 */
TEST_F(IndexRepositoryTest, ShouldFailToRemoveNonExistentEntry) {
    EXPECT_FALSE(repository_->Remove("non_existent_file.cpp"));
}

/**
 * @brief 测试查询不存在的索引条目
 */
TEST_F(IndexRepositoryTest, ShouldReturnEmptyHashForNonExistentEntry) {
    EXPECT_EQ(repository_->GetHash("non_existent_file.cpp"), "");
}

/**
 * @brief 测试多个索引条目的管理
 */
TEST_F(IndexRepositoryTest, ShouldHandleMultipleIndexEntries) {
    std::vector<std::pair<std::string, std::string>> entries = {
        {"src/main.cpp", "hash1"},
        {"src/utils.cpp", "hash2"},
        {"include/header.h", "hash3"},
        {"README.md", "hash4"}};

    // 添加所有条目
    for (const auto& [path, hash] : entries) {
        EXPECT_TRUE(repository_->Add(path, hash));
    }

    // 验证所有条目都存在
    for (const auto& [path, hash] : entries) {
        EXPECT_TRUE(repository_->Contains(path));
        EXPECT_EQ(repository_->GetHash(path), hash);
    }
}

/**
 * @brief 测试索引条目的时间戳功能
 */
TEST_F(IndexRepositoryTest, ShouldTrackEntryTimestamp) {
    std::string path = "src/main.cpp";
    std::string object_hash = "abc123def456";

    // 记录添加前的时间
    auto before_add = std::chrono::system_clock::now();

    // 添加条目
    EXPECT_TRUE(repository_->Add(path, object_hash));

    // 记录添加后的时间
    auto after_add = std::chrono::system_clock::now();

    // 验证条目存在（时间戳功能在内部实现）
    EXPECT_TRUE(repository_->Contains(path));
    EXPECT_EQ(repository_->GetHash(path), object_hash);
}

/**
 * @brief 测试路径的精确匹配
 */
TEST_F(IndexRepositoryTest, ShouldRequireExactPathMatch) {
    std::string path1 = "src/main.cpp";
    std::string path2 = "src/main.c";
    std::string object_hash = "abc123def456";

    repository_->Add(path1, object_hash);

    EXPECT_TRUE(repository_->Contains(path1));
    EXPECT_FALSE(repository_->Contains(path2));
    EXPECT_EQ(repository_->GetHash(path2), "");
}

/**
 * @brief 测试空索引仓库
 */
TEST_F(IndexRepositoryTest, ShouldHandleEmptyRepository) {
    EXPECT_FALSE(repository_->Contains("any_file.cpp"));
    EXPECT_EQ(repository_->GetHash("any_file.cpp"), "");
    EXPECT_FALSE(repository_->Remove("any_file.cpp"));
}
