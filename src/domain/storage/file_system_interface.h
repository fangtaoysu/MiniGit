#pragma once

#include <memory>
#include <string>
#include <vector>

namespace minigit::domain::storage {

// 文件对象元数据
struct FileMetadata {
    std::string hash;      // 对象哈希
    std::string name;      // 文件名
    size_t size = 0;       // 文件大小
    time_t timestamp = 0;  // 修改时间
};

class FileSystemInterface {
public:
    virtual ~FileSystemInterface() = default;

    // 文件操作
    virtual bool StoreFile(const std::string& path,
                           const std::string& content) = 0;
    virtual std::string ReadFile(const std::string& hash) = 0;
    virtual bool DeleteFile(const std::string& hash) = 0;

    // 元数据查询
    virtual std::vector<FileMetadata> ListFiles() const = 0;
    virtual bool FileExists(const std::string& hash) const = 0;

protected:
    FileSystemInterface() = default;
};

}  // namespace minigit::domain::storage
