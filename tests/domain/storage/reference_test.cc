#include <gtest/gtest.h>

#include "domain/storage/repositories/reference_repository.h"

using namespace minigit::domain::storage::repositories;

class ReferenceRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        repository_ = std::make_unique<ReferenceRepository>();
    }

    void TearDown() override { repository_.reset(); }

    std::unique_ptr<ReferenceRepository> repository_;
};

/**
 * @brief 测试设置和获取分支引用
 */
TEST_F(ReferenceRepositoryTest, ShouldSetAndGetBranchReference) {
    std::string ref_name = "refs/heads/main";
    std::string target_hash = "abc123def456789";

    EXPECT_TRUE(
        repository_->Set(ref_name, target_hash, ReferenceType::kBranch));
    EXPECT_EQ(repository_->Get(ref_name), target_hash);
}

/**
 * @brief 测试设置和获取标签引用
 */
TEST_F(ReferenceRepositoryTest, ShouldSetAndGetTagReference) {
    std::string ref_name = "refs/tags/v1.0";
    std::string target_hash = "def456abc123789";

    EXPECT_TRUE(repository_->Set(ref_name, target_hash, ReferenceType::kTag));
    EXPECT_EQ(repository_->Get(ref_name), target_hash);
}

/**
 * @brief 测试设置和获取HEAD引用
 */
TEST_F(ReferenceRepositoryTest, ShouldSetAndGetHeadReference) {
    std::string ref_name = "HEAD";
    std::string target_hash = "ghi789def123456";

    EXPECT_TRUE(repository_->Set(ref_name, target_hash, ReferenceType::kHead));
    EXPECT_EQ(repository_->Get(ref_name), target_hash);
}

/**
 * @brief 测试设置空引用名称应该失败
 */
TEST_F(ReferenceRepositoryTest, ShouldFailToSetEmptyReferenceName) {
    std::string target_hash = "abc123def456";

    EXPECT_FALSE(repository_->Set("", target_hash, ReferenceType::kBranch));
}

/**
 * @brief 测试设置空目标哈希应该失败
 */
TEST_F(ReferenceRepositoryTest, ShouldFailToSetEmptyTargetHash) {
    std::string ref_name = "refs/heads/main";

    EXPECT_FALSE(repository_->Set(ref_name, "", ReferenceType::kBranch));
}

/**
 * @brief 测试删除引用
 */
TEST_F(ReferenceRepositoryTest, ShouldRemoveReference) {
    std::string ref_name = "refs/heads/main";
    std::string target_hash = "abc123def456";

    // 设置引用
    repository_->Set(ref_name, target_hash, ReferenceType::kBranch);
    EXPECT_TRUE(repository_->Exists(ref_name));

    // 删除引用
    EXPECT_TRUE(repository_->Remove(ref_name));
    EXPECT_FALSE(repository_->Exists(ref_name));
    EXPECT_EQ(repository_->Get(ref_name), "");
}

/**
 * @brief 测试删除不存在的引用
 */
TEST_F(ReferenceRepositoryTest, ShouldFailToRemoveNonExistentReference) {
    EXPECT_FALSE(repository_->Remove("refs/heads/non_existent"));
}

/**
 * @brief 测试查询不存在的引用
 */
TEST_F(ReferenceRepositoryTest,
       ShouldReturnEmptyStringForNonExistentReference) {
    EXPECT_EQ(repository_->Get("refs/heads/non_existent"), "");
}

/**
 * @brief 测试更新已存在的引用
 */
TEST_F(ReferenceRepositoryTest, ShouldUpdateExistingReference) {
    std::string ref_name = "refs/heads/main";
    std::string target_hash1 = "abc123def456";
    std::string target_hash2 = "def456abc123";

    // 设置初始引用
    EXPECT_TRUE(
        repository_->Set(ref_name, target_hash1, ReferenceType::kBranch));
    EXPECT_EQ(repository_->Get(ref_name), target_hash1);

    // 更新引用
    EXPECT_TRUE(
        repository_->Set(ref_name, target_hash2, ReferenceType::kBranch));
    EXPECT_EQ(repository_->Get(ref_name), target_hash2);
}

/**
 * @brief 测试多个引用的管理
 */
TEST_F(ReferenceRepositoryTest, ShouldHandleMultipleReferences) {
    std::vector<std::tuple<std::string, std::string, ReferenceType>>
        references = {{"refs/heads/main", "hash1", ReferenceType::kBranch},
                      {"refs/heads/develop", "hash2", ReferenceType::kBranch},
                      {"refs/tags/v1.0", "hash3", ReferenceType::kTag},
                      {"refs/tags/v2.0", "hash4", ReferenceType::kTag},
                      {"HEAD", "hash5", ReferenceType::kHead}};

    // 设置所有引用
    for (const auto& [ref_name, target_hash, type] : references) {
        EXPECT_TRUE(repository_->Set(ref_name, target_hash, type));
    }

    // 验证所有引用都存在
    for (const auto& [ref_name, target_hash, type] : references) {
        EXPECT_TRUE(repository_->Exists(ref_name));
        EXPECT_EQ(repository_->Get(ref_name), target_hash);
    }
}

/**
 * @brief 测试引用名称的精确匹配
 */
TEST_F(ReferenceRepositoryTest, ShouldRequireExactReferenceNameMatch) {
    std::string ref_name1 = "refs/heads/main";
    std::string ref_name2 = "refs/heads/mainn";
    std::string target_hash = "abc123def456";

    repository_->Set(ref_name1, target_hash, ReferenceType::kBranch);

    EXPECT_TRUE(repository_->Exists(ref_name1));
    EXPECT_FALSE(repository_->Exists(ref_name2));
    EXPECT_EQ(repository_->Get(ref_name2), "");
}

/**
 * @brief 测试空引用仓库
 */
TEST_F(ReferenceRepositoryTest, ShouldHandleEmptyRepository) {
    EXPECT_FALSE(repository_->Exists("any_reference"));
    EXPECT_EQ(repository_->Get("any_reference"), "");
    EXPECT_FALSE(repository_->Remove("any_reference"));
}

/**
 * @brief 测试引用类型的独立性
 */
TEST_F(ReferenceRepositoryTest,
       ShouldHandleDifferentReferenceTypesIndependently) {
    std::string ref_name = "refs/heads/main";
    std::string target_hash1 = "abc123def456";
    std::string target_hash2 = "def456abc123";

    // 设置为分支
    EXPECT_TRUE(
        repository_->Set(ref_name, target_hash1, ReferenceType::kBranch));
    EXPECT_EQ(repository_->Get(ref_name), target_hash1);

    // 更新为标签（相同的引用名称，不同的类型）
    EXPECT_TRUE(repository_->Set(ref_name, target_hash2, ReferenceType::kTag));
    EXPECT_EQ(repository_->Get(ref_name), target_hash2);
}
