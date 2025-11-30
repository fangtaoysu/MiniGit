#include "application/init/init_service.h"

#include <filesystem>

#include "infrastructure/logging/logger.h"

namespace minigit::application::init {

InitService::InitService(
    std::shared_ptr<domain::storage::repositories::ObjectRepository> object_repo,
    std::shared_ptr<domain::storage::repositories::ReferenceRepository> ref_repo,
    std::shared_ptr<domain::storage::repositories::IndexRepository> index_repo,
    std::shared_ptr<domain::storage::repositories::RepositoryInitializer> repo_initializer)
    : object_repository_(std::move(object_repo)),
      reference_repository_(std::move(ref_repo)),
      index_repository_(std::move(index_repo)),
      repository_initializer_(std::move(repo_initializer)) {
}

bool InitService::InitializeRepository(const std::string& repo_path, 
                                      const std::string& default_branch) {
    LOG_INFO("Initializing repository at path: " << repo_path);
    
    // 1. 检查是否已经是仓库
    if (IsRepositoryInitialized(repo_path)) {
        LOG_WARN("Repository already initialized at: " << repo_path);
        return false;
    }
    
    // 2. 初始化数据库表结构
    if (!repository_initializer_->InitializeTables()) {
        LOG_ERROR("Failed to initialize database tables");
        return false;
    }
    LOG_INFO("Database tables initialized successfully");
    
    // 3. 创建默认分支引用（指向空commit，暂时不创建commit）
    // 在Git中，初始仓库的HEAD可以指向一个不存在的分支，第一次commit时再创建
    std::string branch_ref = "refs/heads/" + default_branch;
    if (!reference_repository_->CreateBranch(default_branch, "")) {
        LOG_ERROR("Failed to create default branch: " << default_branch);
        return false;
    }
    LOG_INFO("Created default branch: " << default_branch);
    
    // 4. 设置HEAD指向默认分支
    // 注意：此时还没有commit，所以HEAD指向分支名而不是commit SHA1
    // 这里我们通过创建分支时设置HEAD
    std::string head_target = "refs/heads/" + default_branch;
    // 由于ReferenceRepository接口限制，我们需要通过ResolveReference来设置HEAD
    // 但更好的方式是直接更新HEAD引用
    // 暂时先创建分支，HEAD会在第一次commit时自动更新
    
    // 5. 清空暂存区（确保干净状态）
    index_repository_->ClearStaged();
    
    LOG_INFO("Repository initialized successfully at: " << repo_path);
    return true;
}

bool InitService::IsRepositoryInitialized(const std::string& repo_path) {
    // 检查数据库表是否已初始化
    return repository_initializer_->IsRepositoryInitialized();
}

} // namespace minigit::application::init
