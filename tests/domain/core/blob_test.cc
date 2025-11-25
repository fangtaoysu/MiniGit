#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "domain/core/blob.h"

using namespace domain::core;

class DomainCoreBlobTest : public ::testing::Test {
protected:
    std::array<uint8_t, 20> MakeData() {
        std::array<uint8_t, 20> a{};
        for (size_t i = 0; i < a.size(); ++i) a[i] = static_cast<uint8_t>(i);
        return a;
    }
};

TEST_F(DomainCoreBlobTest, CalculateSha1ReturnsHex40) {
    Blob b(MakeData());
    std::string h = b.CalculateSha1();
    ASSERT_EQ(h.size(), 40u);
    for (char c : h) {
        ASSERT_TRUE((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }
}

TEST_F(DomainCoreBlobTest, SerializeReturnsOriginalBytes) {
    auto data = MakeData();
    Blob b(data);
    auto s = b.Serialize();
    ASSERT_EQ(s.size(), data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(s[i], data[i]);
    }
}

TEST_F(DomainCoreBlobTest, DeserializeRestoresBlobFromBytes) {
    auto data = MakeData();
    auto span = std::span<const uint8_t>(data.data(), data.size());
    Blob b = Blob::Deserialize(span);
    auto s = b.Serialize();
    ASSERT_EQ(s.size(), data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(s[i], data[i]);
    }
}

TEST_F(DomainCoreBlobTest, CreateFromFileBuildsBlobWithFileBytes) {
    auto tmp = std::filesystem::temp_directory_path() / "blob_test.bin";
    {
        std::ofstream ofs(tmp, std::ios::binary);
        for (uint8_t i = 0; i < 20; ++i) ofs.put(static_cast<char>(i));
    }
    Blob b = Blob::CreateFromFile(tmp.string());
    auto s = b.Serialize();
    ASSERT_EQ(s.size(), 20u);
    for (size_t i = 0; i < s.size(); ++i) {
        EXPECT_EQ(s[i], static_cast<uint8_t>(i));
    }
    std::filesystem::remove(tmp);
}
