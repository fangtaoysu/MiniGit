#pragma once

#include <chrono>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace minigit::domain::core {

struct CommitMetadata {
    std::string tree_sha1;
    std::vector<std::string> parent_commits;
    std::string author;
    std::string committer;
    std::string message;
};

/**
 * Commit 对象：记录一次提交的元信息与历史关系。
 * - 指向一个顶层 Tree（快照）
 * - 记录零个或多个父提交（形成历史）
 * - 包含作者/提交者身份、时间与提交消息
 * - 序列化为标准 header + 空行 + 消息体
 */
class Commit {
public:
    Commit(const CommitMetadata& metadata);

    const std::string& GetSha1() const;
    const std::string& GetTreeSha1() const;
    const std::vector<std::string>& GetParentCommits() const;
    const std::string& GetAuthor() const;
    const std::string& GetCommitter() const;
    const std::string& GetMessage() const;

    std::vector<uint8_t> Serialize() const;
    static Commit Deserialize(std::span<const uint8_t> Data);

private:
    std::string CalculateSha1() const;

    const std::string ObjType = "commit";
    CommitMetadata meta_data_;
    std::string sha1_;
};

}  // namespace minigit::domain::core