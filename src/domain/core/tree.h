#pragma once

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace minigit::domain::core {

struct TreeEntry {
    std::string mode;
    std::string obj_type;
    std::string sha1;
    std::string name;
};

/**
 * Tree 对象：表示目录层级的快照，由若干 TreeEntry 组成。
 * - 每个条目关联文件名/子目录与对应对象（Blob 或子 Tree）
 * - 负责组合结构并计算快照的 SHA-1
 * - Commit 指向顶层 Tree 以表示一次提交的文件快照
 */
class Tree {
public:
    Tree();
    explicit Tree(std::vector<TreeEntry> entries);

    // 添加条目返回新对象（函数式风格）
    Tree AddEntry(const std::string& mode, const std::string& obj_type,
                  const std::string& sha1, const std::string& name) const;

    std::string CalculateSha1() const;
    std::string GetSha1() const;
    static std::string Deserialize(std::span<const uint8_t> data);
    std::vector<uint8_t> Serialize() const;

private:
    std::string obj_type_;
    std::string sha1_;
    std::vector<TreeEntry> entries_;
};

}  // namespace minigit::domain::core