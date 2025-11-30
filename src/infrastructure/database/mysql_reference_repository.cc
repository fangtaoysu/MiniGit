#include "infrastructure/database/mysql_reference_repository.h"

#include "infrastructure/database/database_manager.h"
#include "infrastructure/logging/logger.h"

namespace minigit::infrastructure::database {

MySQLReferenceRepository::MySQLReferenceRepository() {
}

bool MySQLReferenceRepository::UpdateHead(const std::string& commit_sha1) {
    return CreateOrUpdateReference("HEAD", commit_sha1, "head");
}

std::string MySQLReferenceRepository::GetHead() {
    return ResolveReference("HEAD");
}

bool MySQLReferenceRepository::CreateBranch(const std::string& branch_name, 
                                           const std::string& commit_sha1) {
    std::string ref_name = "refs/heads/" + branch_name;
    bool success = CreateOrUpdateReference(ref_name, commit_sha1, "branch");
    
    if (success && commit_sha1.empty()) {
        // 如果是空分支（初始化时），设置HEAD指向这个分支
        // 通过符号引用：HEAD -> refs/heads/branch_name
        std::string sql = R"(
            INSERT INTO `references` (name, target, type) 
            VALUES ('HEAD', ?, 'head')
            ON DUPLICATE KEY UPDATE target = ?
        )";
        auto& db_manager = DbManager::GetInstance();
        db_manager.Execute(sql, {ref_name, ref_name});
    }
    
    return success;
}

bool MySQLReferenceRepository::DeleteBranch(const std::string& branch_name) {
    std::string ref_name = "refs/heads/" + branch_name;
    std::string sql = "DELETE FROM `references` WHERE name = ?";
    
    auto& db_manager = DbManager::GetInstance();
    int64_t result = db_manager.Execute(sql, {ref_name});
    
    return result >= 0;
}

std::string MySQLReferenceRepository::GetBranchHead(const std::string& branch_name) {
    std::string ref_name = "refs/heads/" + branch_name;
    return ResolveReference(ref_name);
}

bool MySQLReferenceRepository::CreateTag(const std::string& tag_name, 
                                        const std::string& commit_sha1) {
    std::string ref_name = "refs/tags/" + tag_name;
    return CreateOrUpdateReference(ref_name, commit_sha1, "tag");
}

std::string MySQLReferenceRepository::ResolveReference(const std::string& ref_name) {
    std::string sql = "SELECT target FROM `references` WHERE name = ?";
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql, {ref_name});
    
    if (results.empty()) {
        return "";
    }
    
    const auto& row = results[0];
    auto it = row.find("target");
    if (it != row.end()) {
        std::string target = it->second;
        
        // 如果是符号引用（以refs/开头），递归解析
        if (target.find("refs/") == 0) {
            return ResolveReference(target);
        }
        
        return target;
    }
    
    return "";
}

bool MySQLReferenceRepository::CreateOrUpdateReference(const std::string& name, 
                                                       const std::string& target, 
                                                       const std::string& type) {
    std::string sql = R"(
        INSERT INTO `references` (name, target, type) 
        VALUES (?, ?, ?)
        ON DUPLICATE KEY UPDATE target = ?, updated_at = CURRENT_TIMESTAMP
    )";
    
    auto& db_manager = DbManager::GetInstance();
    int64_t result = db_manager.Execute(sql, {name, target, type, target});
    
    if (result < 0) {
        LOG_ERROR("Failed to create/update reference: " << name);
        return false;
    }
    
    LOG_DEBUG("Created/updated reference: " << name << " -> " << target);
    return true;
}

} // namespace minigit::infrastructure::database
