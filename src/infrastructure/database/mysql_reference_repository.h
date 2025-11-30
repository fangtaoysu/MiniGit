#pragma once

#include "domain/storage/repositories/reference_repository.h"

namespace minigit::infrastructure::database {

/**
 * @brief MySQL实现的ReferenceRepository
 * 负责管理Git引用（HEAD、分支、标签）
 */
class MySQLReferenceRepository : public domain::storage::repositories::ReferenceRepository {
public:
    MySQLReferenceRepository();
    
    bool UpdateHead(const std::string& commit_sha1) override;
    std::string GetHead() override;
    
    bool CreateBranch(const std::string& branch_name, const std::string& commit_sha1) override;
    bool DeleteBranch(const std::string& branch_name) override;
    std::string GetBranchHead(const std::string& branch_name) override;
    
    bool CreateTag(const std::string& tag_name, const std::string& commit_sha1) override;
    std::string ResolveReference(const std::string& ref_name) override;

private:
    /**
     * @brief 创建或更新引用
     */
    bool CreateOrUpdateReference(const std::string& name, 
                                 const std::string& target, 
                                 const std::string& type);
};

} // namespace minigit::infrastructure::database
