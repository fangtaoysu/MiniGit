#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "domain/core/blob.h"

namespace minigit::domain::storage::repositories {

class ObjectRepository {
public:
    ObjectRepository() = default;
    ~ObjectRepository() = default;

    // 对象存储 - 使用core中的Blob对象
    bool Store(const std::string& hash,
               std::shared_ptr<minigit::domain::core::Blob> blob);
    std::shared_ptr<minigit::domain::core::Blob> Retrieve(
        const std::string& hash) const;
    bool Remove(const std::string& hash);
    bool Exists(const std::string& hash) const;

    // 兼容接口 - 字符串内容存储
    bool Store(const std::string& hash, const std::string& content);
    std::string RetrieveContent(const std::string& hash) const;

private:
    std::unordered_map<std::string,
                       std::shared_ptr<minigit::domain::core::Blob>>
        objects_;

    // 跟踪字符串内容的实际长度
    std::unordered_map<std::string, size_t> content_lengths_;
};

}  // namespace minigit::domain::storage::repositories
