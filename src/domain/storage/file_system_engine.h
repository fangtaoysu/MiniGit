#pragma once

#include <memory>

#include "file_system_interface.h"
#include "repositories/index_repository.h"
#include "repositories/object_repository.h"
#include "repositories/reference_repository.h"

namespace minigit::domain::storage {

class FileSystemEngine : public FileSystemInterface {
public:
    FileSystemEngine();
    ~FileSystemEngine() override = default;

    // 实现接口方法
    bool StoreFile(const std::string& path,
                   const std::string& content) override;
    std::string ReadFile(const std::string& hash) override;
    bool DeleteFile(const std::string& hash) override;

    std::vector<FileMetadata> ListFiles() const override;
    bool FileExists(const std::string& hash) const override;

    // 获取仓库引用（供上层使用）
    repositories::IndexRepository& GetIndexRepository() { return index_repo_; }
    repositories::ObjectRepository& GetObjectRepository() {
        return object_repo_;
    }
    repositories::ReferenceRepository& GetReferenceRepository() {
        return ref_repo_;
    }

private:
    // 内部辅助方法
    std::string GenerateHash(const std::string& content) const;

    repositories::IndexRepository index_repo_;
    repositories::ObjectRepository object_repo_;
    repositories::ReferenceRepository ref_repo_;
};

}  // namespace minigit::domain::storage
