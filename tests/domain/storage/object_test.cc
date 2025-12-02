#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <memory>

#include "domain/core/blob.h"
#include "domain/storage/repositories/object_repository.h"

using namespace minigit::domain::storage::repositories;

class ObjectRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        repository_ = std::make_unique<ObjectRepository>();
    }

    void TearDown() override { repository_.reset(); }

    std::unique_ptr<ObjectRepository> repository_;
};

/**
 * @brief 测试存储和检索Blob对象
 */
TEST_F(ObjectRepositoryTest, ShouldStoreAndRetrieveBlob) {
    // 创建测试数据
    std::array<uint8_t, 20> test_data{};
    for (int i = 0; i < 20; ++i) {
        test_data[i] = static_cast<uint8_t>(i);
    }

    auto blob = std::make_shared<minigit::domain::core::Blob>(test_data);
    std::string hash = blob->GetSha1();

    // 存储对象
    EXPECT_TRUE(repository_->Store(hash, blob));

    // 验证对象存在
    EXPECT_TRUE(repository_->Exists(hash));

    // 检索对象
    auto retrieved_blob = repository_->Retrieve(hash);
    EXPECT_NE(retrieved_blob, nullptr);
    EXPECT_EQ(retrieved_blob->GetSha1(), hash);
}

/**
 * @brief 测试存储空哈希应该失败
 */
TEST_F(ObjectRepositoryTest, ShouldFailToStoreEmptyHash) {
    std::array<uint8_t, 20> test_data{};
    auto blob = std::make_shared<minigit::domain::core::Blob>(test_data);

    EXPECT_FALSE(repository_->Store("", blob));
}

/**
 * @brief 测试存储空Blob应该失败
 */
TEST_F(ObjectRepositoryTest, ShouldFailToStoreNullBlob) {
    EXPECT_FALSE(repository_->Store(
        "some_hash", std::shared_ptr<minigit::domain::core::Blob>(nullptr)));
}

/**
 * @brief 测试检索不存在的对象
 */
TEST_F(ObjectRepositoryTest, ShouldReturnNullForNonExistentObject) {
    auto retrieved_blob = repository_->Retrieve("non_existent_hash");
    EXPECT_EQ(retrieved_blob, nullptr);
}

/**
 * @brief 测试删除对象
 */
TEST_F(ObjectRepositoryTest, ShouldRemoveObject) {
    // 创建并存储对象
    std::array<uint8_t, 20> test_data{};
    auto blob = std::make_shared<minigit::domain::core::Blob>(test_data);
    std::string hash = blob->GetSha1();

    repository_->Store(hash, blob);
    EXPECT_TRUE(repository_->Exists(hash));

    // 删除对象
    EXPECT_TRUE(repository_->Remove(hash));
    EXPECT_FALSE(repository_->Exists(hash));
}

/**
 * @brief 测试删除不存在的对象
 */
TEST_F(ObjectRepositoryTest, ShouldFailToRemoveNonExistentObject) {
    EXPECT_FALSE(repository_->Remove("non_existent_hash"));
}

/**
 * @brief 测试字符串内容的兼容接口
 */
TEST_F(ObjectRepositoryTest, ShouldStoreAndRetrieveStringContent) {
    std::string content = "Hello, Git!";
    std::string hash = "test_hash_123";

    // 存储字符串内容
    EXPECT_TRUE(repository_->Store(hash, content));

    // 验证存在性
    EXPECT_TRUE(repository_->Exists(hash));

    // 检索内容
    std::string retrieved_content = repository_->RetrieveContent(hash);
    EXPECT_EQ(retrieved_content, content);
}

/**
 * @brief 测试检索不存在的字符串内容
 */
TEST_F(ObjectRepositoryTest, ShouldReturnEmptyStringForNonExistentContent) {
    std::string retrieved_content =
        repository_->RetrieveContent("non_existent_hash");
    EXPECT_EQ(retrieved_content, "");
}

/**
 * @brief 测试覆盖已存在的对象
 */
TEST_F(ObjectRepositoryTest, ShouldOverwriteExistingObject) {
    std::array<uint8_t, 20> test_data1{};
    std::array<uint8_t, 20> test_data2{};

    // 填充不同的数据
    for (int i = 0; i < 20; ++i) {
        test_data1[i] = static_cast<uint8_t>(i);
        test_data2[i] = static_cast<uint8_t>(i + 100);
    }

    auto blob1 = std::make_shared<minigit::domain::core::Blob>(test_data1);
    auto blob2 = std::make_shared<minigit::domain::core::Blob>(test_data2);
    std::string hash = blob1->GetSha1();

    // 存储第一个对象
    EXPECT_TRUE(repository_->Store(hash, blob1));
    auto retrieved1 = repository_->Retrieve(hash);
    EXPECT_EQ(retrieved1->GetSha1(), blob1->GetSha1());

    // 覆盖存储第二个对象（使用相同的哈希）
    EXPECT_TRUE(repository_->Store(hash, blob2));
    auto retrieved2 = repository_->Retrieve(hash);
    EXPECT_EQ(retrieved2->GetSha1(), blob2->GetSha1());
}

/**
 * @brief 测试多个对象的存储和检索
 */
TEST_F(ObjectRepositoryTest, ShouldHandleMultipleObjects) {
    const int num_objects = 5;
    std::vector<std::shared_ptr<minigit::domain::core::Blob>> blobs;
    std::vector<std::string> hashes;

    // 创建多个对象
    for (int i = 0; i < num_objects; ++i) {
        std::array<uint8_t, 20> test_data{};
        test_data[0] = static_cast<uint8_t>(i);
        test_data[1] = static_cast<uint8_t>(i + 50);

        auto blob = std::make_shared<minigit::domain::core::Blob>(test_data);
        blobs.push_back(blob);
        hashes.push_back(blob->GetSha1());
    }

    // 存储所有对象
    for (int i = 0; i < num_objects; ++i) {
        EXPECT_TRUE(repository_->Store(hashes[i], blobs[i]));
    }

    // 验证所有对象都存在
    for (int i = 0; i < num_objects; ++i) {
        EXPECT_TRUE(repository_->Exists(hashes[i]));
        auto retrieved = repository_->Retrieve(hashes[i]);
        EXPECT_NE(retrieved, nullptr);
        EXPECT_EQ(retrieved->GetSha1(), hashes[i]);
    }
}
