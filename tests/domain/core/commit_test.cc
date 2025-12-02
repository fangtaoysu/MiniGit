#include "domain/core/commit.h"

#include <openssl/sha.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"

namespace {
std::vector<uint8_t> ToBytes(const std::string& s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}
}  // namespace

using namespace minigit::domain::core;

TEST(CommitTest, DeserializeParsesFields) {
    std::string text =
        "tree abcdef1234567890abcdef1234567890abcdef12\n"
        "parent 1111111111111111111111111111111111111111\n"
        "parent 2222222222222222222222222222222222222222\n"
        "author Alice <alice@example.com>\n"
        "committer Bob <bob@example.com>\n"
        "\n"
        "Initial commit message\nSecond line\n";
    auto data = ToBytes(text);
    auto c = Commit::Deserialize(std::span<const uint8_t>(data));

    EXPECT_EQ(c.GetTreeSha1(), "abcdef1234567890abcdef1234567890abcdef12");
    ASSERT_EQ(c.GetParentCommits().size(), 2u);
    EXPECT_EQ(c.GetParentCommits()[0],
              "1111111111111111111111111111111111111111");
    EXPECT_EQ(c.GetParentCommits()[1],
              "2222222222222222222222222222222222222222");
    EXPECT_EQ(c.GetAuthor(), "Alice <alice@example.com>");
    EXPECT_EQ(c.GetCommitter(), "Bob <bob@example.com>");
    EXPECT_EQ(c.GetMessage(), "Initial commit message\nSecond line\n");
}

TEST(CommitTest, SerializeThenDeserializeRoundTripPreservesFields) {
    std::string text =
        "tree aaaabbbbccccddddeeeeffff0000111122223333\n"
        "parent 9999999999999999999999999999999999999999\n"
        "author Carol <carol@example.com>\n"
        "committer Carol <carol@example.com>\n"
        "\n"
        "Msg\n";
    auto data = ToBytes(text);
    auto c1 = Commit::Deserialize(std::span<const uint8_t>(data));
    auto ser = c1.Serialize();
    auto c2 = Commit::Deserialize(std::span<const uint8_t>(ser));

    EXPECT_EQ(c2.GetTreeSha1(), c1.GetTreeSha1());
    EXPECT_EQ(c2.GetParentCommits(), c1.GetParentCommits());
    EXPECT_EQ(c2.GetAuthor(), c1.GetAuthor());
    EXPECT_EQ(c2.GetCommitter(), c1.GetCommitter());
    EXPECT_EQ(c2.GetMessage(), c1.GetMessage());
}

TEST(CommitTest, NoParentsHandled) {
    std::string text =
        "tree 0123456789abcdef0123456789abcdef01234567\n"
        "author Dave <dave@example.com>\n"
        "committer Dave <dave@example.com>\n"
        "\n"
        "Solo\n";
    auto c = Commit::Deserialize(std::span<const uint8_t>(ToBytes(text)));
    EXPECT_TRUE(c.GetParentCommits().empty());
    EXPECT_EQ(c.GetMessage(), "Solo\n");
}

TEST(CommitTest, MessageTrailingNewlineBehavior) {
    std::string text1 =
        "tree 0123456789abcdef0123456789abcdef01234567\n"
        "author Eve <eve@example.com>\n"
        "committer Eve <eve@example.com>\n"
        "\n"
        "Has newline\n";
    auto c1 = Commit::Deserialize(std::span<const uint8_t>(ToBytes(text1)));
    auto s1 = c1.Serialize();
    auto c1b = Commit::Deserialize(std::span<const uint8_t>(s1));
    EXPECT_EQ(c1b.GetMessage(), "Has newline\n");

    std::string text2 =
        "tree 0123456789abcdef0123456789abcdef01234567\n"
        "author Eve <eve@example.com>\n"
        "committer Eve <eve@example.com>\n"
        "\n"
        "No newline";
    auto c2 = Commit::Deserialize(std::span<const uint8_t>(ToBytes(text2)));
    auto s2 = c2.Serialize();
    auto c2b = Commit::Deserialize(std::span<const uint8_t>(s2));
    EXPECT_EQ(c2b.GetMessage(), "No newline\n");
}
