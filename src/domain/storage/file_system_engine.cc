#include "domain/storage/file_system_engine.h"

#include <functional>
#include <sstream>

#include "domain/core/hash_util.h"

namespace minigit::domain::storage {

FileSystemEngine::FileSystemEngine() = default;

bool FileSystemEngine::StoreFile(const std::string& path,
                                 const std::string& content) {
    // 生成内容哈希作为对象ID
    std::string hash = GenerateHash(content);

    // 存储对象（使用兼容接口）
    if (!object_repo_.Store(hash, content)) {
        return false;
    }

    // 更新索引
    return index_repo_.Add(path, hash);
}

std::string FileSystemEngine::ReadFile(const std::string& hash) {
    return object_repo_.RetrieveContent(hash);
}

bool FileSystemEngine::DeleteFile(const std::string& hash) {
    // 注意：这里只是简单删除对象，实际git需要处理索引和引用
    return object_repo_.Remove(hash);
}

std::vector<FileMetadata> FileSystemEngine::ListFiles() const {
    std::vector<FileMetadata> result;
    // 注意：这里简化了实现，实际需要遍历索引
    return result;
}

bool FileSystemEngine::FileExists(const std::string& hash) const {
    return object_repo_.Exists(hash);
}

std::string FileSystemEngine::GenerateHash(const std::string& content) const {
    // 使用core中的SHA-1实现，符合Git标准
    return minigit::domain::core::HashUtil::Sha1Hex(content);
}

}  // namespace minigit::domain::storage