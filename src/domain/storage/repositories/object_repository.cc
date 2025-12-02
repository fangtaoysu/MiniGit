#include "domain/storage/repositories/object_repository.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>

#include "domain/core/blob.h"

namespace minigit::domain::storage::repositories {

// 使用Blob对象的存储接口
bool ObjectRepository::Store(
    const std::string& hash,
    std::shared_ptr<minigit::domain::core::Blob> blob) {
    if (hash.empty() || !blob) {
        return false;
    }
    objects_[hash] = blob;
    return true;
}

std::shared_ptr<minigit::domain::core::Blob> ObjectRepository::Retrieve(
    const std::string& hash) const {
    auto it = objects_.find(hash);
    if (it == objects_.end()) {
        return nullptr;
    }
    return it->second;
}

bool ObjectRepository::Remove(const std::string& hash) {
    return objects_.erase(hash) > 0;
}

bool ObjectRepository::Exists(const std::string& hash) const {
    return objects_.find(hash) != objects_.end();
}

// 兼容接口 - 字符串内容存储
bool ObjectRepository::Store(const std::string& hash,
                             const std::string& content) {
    if (hash.empty()) {
        return false;
    }

    // 将字符串内容转换为Blob对象
    std::array<uint8_t, 20> data{};
    size_t copy_size = std::min(content.size(), size_t(20));
    for (size_t i = 0; i < copy_size; ++i) {
        data[i] = static_cast<uint8_t>(content[i]);
    }
    auto blob = std::make_shared<minigit::domain::core::Blob>(data);

    objects_[hash] = blob;
    content_lengths_[hash] = content.size();  // 记录实际内容长度
    return true;
}

std::string ObjectRepository::RetrieveContent(const std::string& hash) const {
    auto blob = Retrieve(hash);
    if (!blob) {
        return "";
    }

    // 从Blob对象获取内容（使用记录的实际长度）
    auto data = blob->Serialize();
    auto it = content_lengths_.find(hash);
    size_t content_length =
        (it != content_lengths_.end()) ? it->second : data.size();

    return std::string(reinterpret_cast<const char*>(data.data()),
                       std::min(content_length, data.size()));
}

}  // namespace minigit::domain::storage::repositories