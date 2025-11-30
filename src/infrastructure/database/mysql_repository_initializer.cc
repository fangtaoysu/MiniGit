#include "infrastructure/database/mysql_repository_initializer.h"

#include "infrastructure/database/database_manager.h"
#include "infrastructure/logging/logger.h"

namespace minigit::infrastructure::database {

MySQLRepositoryInitializer::MySQLRepositoryInitializer() {
}

bool MySQLRepositoryInitializer::InitializeTables() {
    LOG_INFO("Initializing Git repository database tables...");
    
    // 使用事务确保所有表一起创建
    auto& db_manager = DbManager::GetInstance();
    db_manager.BeginTransaction();
    
    try {
        // 创建所有必需的表
        if (!CreateGitObjectsTable()) {
            LOG_ERROR("Failed to create git_objects table");
            db_manager.RollbackTransaction();
            return false;
        }
        
        if (!CreateReferencesTable()) {
            LOG_ERROR("Failed to create references table");
            db_manager.RollbackTransaction();
            return false;
        }
        
        if (!CreateStagingIndexTable()) {
            LOG_ERROR("Failed to create staging_index table");
            db_manager.RollbackTransaction();
            return false;
        }
        
        if (!CreateCommitHistoryTable()) {
            LOG_ERROR("Failed to create commit_history table");
            db_manager.RollbackTransaction();
            return false;
        }
        
        db_manager.CommitTransaction();
        LOG_INFO("All database tables initialized successfully");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Exception during table initialization: " << e.what());
        db_manager.RollbackTransaction();
        return false;
    }
}

bool MySQLRepositoryInitializer::IsRepositoryInitialized() {
    // 检查核心表是否存在
    return TableExists("git_objects") && 
           TableExists("references") && 
           TableExists("staging_index");
}

bool MySQLRepositoryInitializer::CreateGitObjectsTable() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS git_objects (
            sha1 CHAR(40) PRIMARY KEY,
            type ENUM('blob', 'commit', 'tree') NOT NULL,
            data LONGBLOB NOT NULL,
            size INT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            INDEX idx_type (type),
            INDEX idx_created_at (created_at)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    auto& db_manager = DbManager::GetInstance();
    int64_t result = db_manager.Execute(sql);
    
    if (result < 0) {
        LOG_ERROR("Failed to execute CREATE TABLE git_objects");
        return false;
    }
    
    LOG_INFO("git_objects table created/verified");
    return true;
}

bool MySQLRepositoryInitializer::CreateReferencesTable() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS `references` (
            name VARCHAR(255) PRIMARY KEY,
            target VARCHAR(40) NOT NULL,
            type ENUM('head', 'branch', 'tag') NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
            INDEX idx_type (type),
            INDEX idx_target (target)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    auto& db_manager = DbManager::GetInstance();
    int64_t result = db_manager.Execute(sql);
    
    if (result < 0) {
        LOG_ERROR("Failed to execute CREATE TABLE references");
        return false;
    }
    
    LOG_INFO("references table created/verified");
    return true;
}

bool MySQLRepositoryInitializer::CreateStagingIndexTable() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS staging_index (
            file_path VARCHAR(767) PRIMARY KEY,
            blob_sha1 CHAR(40) NOT NULL,
            mode VARCHAR(6) NOT NULL DEFAULT '100644',
            staged_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            INDEX idx_blob_sha1 (blob_sha1)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    auto& db_manager = DbManager::GetInstance();
    int64_t result = db_manager.Execute(sql);
    
    if (result < 0) {
        LOG_ERROR("Failed to execute CREATE TABLE staging_index");
        return false;
    }
    
    LOG_INFO("staging_index table created/verified");
    return true;
}

bool MySQLRepositoryInitializer::CreateCommitHistoryTable() {
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS commit_history (
            commit_sha1 CHAR(40) PRIMARY KEY,
            tree_sha1 CHAR(40) NOT NULL,
            parent_commits JSON,
            author VARCHAR(255) NOT NULL,
            committer VARCHAR(255) NOT NULL,
            message TEXT,
            commit_time TIMESTAMP NOT NULL,
            INDEX idx_tree_sha1 (tree_sha1),
            INDEX idx_commit_time (commit_time)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
    )";
    
    auto& db_manager = DbManager::GetInstance();
    int64_t result = db_manager.Execute(sql);
    
    if (result < 0) {
        LOG_ERROR("Failed to execute CREATE TABLE commit_history");
        return false;
    }
    
    LOG_INFO("commit_history table created/verified");
    return true;
}

bool MySQLRepositoryInitializer::TableExists(const std::string& table_name) {
    std::string sql = R"(
        SELECT COUNT(*) as count 
        FROM information_schema.tables 
        WHERE table_schema = DATABASE() 
        AND table_name = ?
    )";
    
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql, {table_name});
    
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

} // namespace minigit::infrastructure::database
