#pragma once

#include <memory>

#include "application/init/init_service.h"
#include "domain/storage/repositories/index_repository.h"
#include "domain/storage/repositories/object_repository.h"
#include "domain/storage/repositories/reference_repository.h"
#include "domain/storage/repositories/repository_initializer.h"
#include "infrastructure/database/mysql_index_repository.h"
#include "infrastructure/database/mysql_object_repository.h"
#include "infrastructure/database/mysql_reference_repository.h"
#include "infrastructure/database/mysql_repository_initializer.h"

namespace minigit::application::init {

/**
 * @brief 依赖注入配置
 * 负责创建和组装InitService及其依赖
 */
class DependencyConfig {
public:
    static std::shared_ptr<domain::storage::repositories::ObjectRepository> 
    CreateObjectRepository() {
        return std::make_shared<infrastructure::database::MySQLObjectRepository>();
    }
    
    static std::shared_ptr<domain::storage::repositories::ReferenceRepository> 
    CreateReferenceRepository() {
        return std::make_shared<infrastructure::database::MySQLReferenceRepository>();
    }
    
    static std::shared_ptr<domain::storage::repositories::IndexRepository> 
    CreateIndexRepository() {
        return std::make_shared<infrastructure::database::MySQLIndexRepository>();
    }
    
    static std::shared_ptr<domain::storage::repositories::RepositoryInitializer> 
    CreateRepositoryInitializer() {
        return std::make_shared<infrastructure::database::MySQLRepositoryInitializer>();
    }
    
    static std::shared_ptr<InitService> CreateInitService() {
        return std::make_shared<InitService>(
            CreateObjectRepository(),
            CreateReferenceRepository(),
            CreateIndexRepository(),
            CreateRepositoryInitializer());
    }
};

} // namespace minigit::application::init
