#pragma once

#include "domain/storage/repositories/repository_initializer.h"

namespace minigit::infrastructure::database {

/**
 * @brief MySQL实现的Repository初始化器
 * 负责创建和管理Git仓库所需的数据库表结构
 */
class MySQLRepositoryInitializer : public domain::storage::repositories::RepositoryInitializer {
public:
    MySQLRepositoryInitializer();
    
    bool InitializeTables() override;
    bool IsRepositoryInitialized() override;

private:
    /**
     * @brief 创建git_objects表
     */
    bool CreateGitObjectsTable();
    
    /**
     * @brief 创建references表
     */
    bool CreateReferencesTable();
    
    /**
     * @brief 创建staging_index表
     */
    bool CreateStagingIndexTable();
    
    /**
     * @brief 创建commit_history表（可选，用于优化查询）
     */
    bool CreateCommitHistoryTable();
    
    /**
     * @brief 检查表是否存在
     */
    bool TableExists(const std::string& table_name);
};

} // namespace minigit::infrastructure::database
