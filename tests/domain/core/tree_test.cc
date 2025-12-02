#include "domain/core/tree.h"

#include <openssl/sha.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"

namespace {
std::string Sha1Hex(const std::string& s) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(s.data()), s.size(), hash);
    static const char* kHex = "0123456789abcdef";
    std::string out;
    out.resize(2 * SHA_DIGEST_LENGTH);
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        out[2 * i] = kHex[(hash[i] >> 4) & 0xF];
        out[2 * i + 1] = kHex[hash[i] & 0xF];
    }
    return out;
}
std::vector<uint8_t> ToBytes(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}
}  // namespace

using namespace minigit::domain::core;

TEST(TreeTest, DeserializeComputesExpectedSha1) {
    std::string ser;
    ser += "100644 blob aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\tfile1.txt\n";
    ser += "100755 blob bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\ttool.sh\n";
    ser += "040000 tree cccccccccccccccccccccccccccccccccccccccc\tsubdir\n";

    auto expected = Sha1Hex(
        "100644 blob aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\tfile1.txt\n"
        "100755 blob bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\ttool.sh\n"
        "040000 tree cccccccccccccccccccccccccccccccccccccccc\tsubdir\n");

    auto sha = Tree::Deserialize(std::span<const uint8_t>(ToBytes(ser)));
    EXPECT_EQ(sha, expected);
}

TEST(TreeTest, AddEntryIsFunctionalAndImmutable) {
    Tree t0;
    auto s0 = t0.CalculateSha1();

    Tree t1 = t0.AddEntry("100644", "blob",
                          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "a.txt");
    auto s1 = t1.CalculateSha1();

    Tree t2 = t1.AddEntry("100755", "blob",
                          "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", "run.sh");
    auto s2 = t2.CalculateSha1();

    EXPECT_NE(s0, s1);
    EXPECT_NE(s1, s2);

    std::string expected_data1 =
        "100644 blob aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\ta.txt\n";
    EXPECT_EQ(s1, Sha1Hex(expected_data1));

    std::string expected_data2 =
        "100644 blob aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\ta.txt\n"
        "100755 blob bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\trun.sh\n";
    EXPECT_EQ(s2, Sha1Hex(expected_data2));
}

TEST(TreeTest, CalculateSha1ConsistencyWithDeserialize) {
    Tree t;
    t = t.AddEntry("100644", "blob", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                   "a.txt");
    t = t.AddEntry("100644", "blob", "cccccccccccccccccccccccccccccccccccccccc",
                   "c.txt");
    auto calc = t.CalculateSha1();

    std::string ser =
        "100644 blob aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\ta.txt\n"
        "100644 blob cccccccccccccccccccccccccccccccccccccccc\tc.txt\n";

    auto des = Tree::Deserialize(std::span<const uint8_t>(ToBytes(ser)));
    EXPECT_EQ(calc, des);
}
