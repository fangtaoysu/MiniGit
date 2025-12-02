#pragma once

#include <string>
#include <unordered_map>

namespace minigit::domain::storage::repositories {

// 索引条目：路径 -> 对象哈希
struct IndexEntry {
    std::string path;
    std::string object_hash;
    time_t timestamp;
};

class IndexRepository {
public:
    IndexRepository() = default;
    ~IndexRepository() = default;

    // 基本索引操作
    bool Add(const std::string& path, const std::string& object_hash);
    bool Remove(const std::string& path);

    // 查询
    std::string GetHash(const std::string& path) const;
    bool Contains(const std::string& path) const;

private:
    std::unordered_map<std::string, IndexEntry> entries_;
};

}  // namespace minigit::domain::storage::repositories
