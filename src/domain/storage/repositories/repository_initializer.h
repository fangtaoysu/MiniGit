#pragma once

#include <string>

namespace minigit::domain::storage::repositories {

/**
 * @brief Repository初始化接口
 * 负责初始化数据库表结构，确保仓库可以正常使用
 */
class RepositoryInitializer {
public:
    virtual ~RepositoryInitializer() = default;
    
    /**
     * @brief 初始化数据库表结构
     * @return true 如果初始化成功，false 如果失败
     */
    virtual bool InitializeTables() = 0;
    
    /**
     * @brief 检查仓库是否已初始化
     * @return true 如果表已存在，false 如果不存在
     */
    virtual bool IsRepositoryInitialized() = 0;
};

} // namespace minigit::domain::storage::repositories
