#include "infrastructure/database/mysql_index_repository.h"

#include "infrastructure/database/database_manager.h"
#include "infrastructure/logging/logger.h"

namespace minigit::infrastructure::database {

MySQLIndexRepository::MySQLIndexRepository() {
}

void MySQLIndexRepository::StageFile(const std::string& file_path, const std::string& blob_sha1) {
    std::string sql = R"(
        INSERT INTO staging_index (file_path, blob_sha1, mode) 
        VALUES (?, ?, '100644')
        ON DUPLICATE KEY UPDATE blob_sha1 = ?, staged_at = CURRENT_TIMESTAMP
    )";
    
    auto& db_manager = DbManager::GetInstance();
    db_manager.Execute(sql, {file_path, blob_sha1, blob_sha1});
    
    LOG_DEBUG("Staged file: " << file_path << " -> " << blob_sha1);
}

void MySQLIndexRepository::UnstageFile(const std::string& file_path) {
    std::string sql = "DELETE FROM staging_index WHERE file_path = ?";
    
    auto& db_manager = DbManager::GetInstance();
    db_manager.Execute(sql, {file_path});
    
    LOG_DEBUG("Unstaged file: " << file_path);
}

bool MySQLIndexRepository::IsStaged(const std::string& file_path) {
    std::string sql = "SELECT COUNT(*) as count FROM staging_index WHERE file_path = ?";
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql, {file_path});
    
    if (results.empty()) {
        return false;
    }
    
    const auto& row = results[0];
    auto it = row.find("count");
    if (it != row.end()) {
        return std::stoi(it->second) > 0;
    }
    
    return false;
}

std::unordered_map<std::string, std::string> MySQLIndexRepository::GetStagedFiles() {
    std::string sql = "SELECT file_path, blob_sha1 FROM staging_index";
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql);
    
    std::unordered_map<std::string, std::string> staged_files;
    for (const auto& row : results) {
        auto path_it = row.find("file_path");
        auto sha1_it = row.find("blob_sha1");
        
        if (path_it != row.end() && sha1_it != row.end()) {
            staged_files[path_it->second] = sha1_it->second;
        }
    }
    
    return staged_files;
}

void MySQLIndexRepository::ClearStaged() {
    std::string sql = "DELETE FROM staging_index";
    
    auto& db_manager = DbManager::GetInstance();
    db_manager.Execute(sql);
    
    LOG_DEBUG("Cleared staging area");
}

} // namespace minigit::infrastructure::database
