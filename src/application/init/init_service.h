#pragma once

#include <memory>
#include <string>

#include "domain/storage/repositories/object_repository.h"
#include "domain/storage/repositories/reference_repository.h"
#include "domain/storage/repositories/index_repository.h"
#include "domain/storage/repositories/repository_initializer.h"

namespace minigit::application::init {

/**
 * @brief Init业务服务
 * 负责初始化Git仓库的核心业务逻辑
 */
class InitService {
public:
    InitService(
        std::shared_ptr<domain::storage::repositories::ObjectRepository> object_repo,
        std::shared_ptr<domain::storage::repositories::ReferenceRepository> ref_repo,
        std::shared_ptr<domain::storage::repositories::IndexRepository> index_repo,
        std::shared_ptr<domain::storage::repositories::RepositoryInitializer> repo_initializer);
    
    /**
     * @brief 初始化Git仓库
     * @param repo_path 仓库路径（可选，默认为当前目录）
     * @param default_branch 默认分支名（默认为"main"）
     * @return true 如果初始化成功，false 如果失败
     */
    bool InitializeRepository(const std::string& repo_path = ".", 
                             const std::string& default_branch = "main");
    
    /**
     * @brief 检查指定路径是否已经是Git仓库
     * @param repo_path 仓库路径
     * @return true 如果已经是仓库，false 如果不是
     */
    bool IsRepositoryInitialized(const std::string& repo_path = ".");

private:
    std::shared_ptr<domain::storage::repositories::ObjectRepository> object_repository_;
    std::shared_ptr<domain::storage::repositories::ReferenceRepository> reference_repository_;
    std::shared_ptr<domain::storage::repositories::IndexRepository> index_repository_;
    std::shared_ptr<domain::storage::repositories::RepositoryInitializer> repository_initializer_;
};

} // namespace minigit::application::init
