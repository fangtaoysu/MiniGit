#pragma once

#include "domain/storage/repositories/index_repository.h"

namespace minigit::infrastructure::database {

/**
 * @brief MySQL实现的IndexRepository
 * 负责管理Git暂存区（staging area）
 */
class MySQLIndexRepository : public domain::storage::repositories::IndexRepository {
public:
    MySQLIndexRepository();
    
    void StageFile(const std::string& file_path, const std::string& blob_sha1) override;
    void UnstageFile(const std::string& file_path) override;
    bool IsStaged(const std::string& file_path) override;
    std::unordered_map<std::string, std::string> GetStagedFiles() override;
    void ClearStaged() override;
};

} // namespace minigit::infrastructure::database
