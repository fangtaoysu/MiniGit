#pragma once

#include "domain/storage/repositories/object_repository.h"

namespace minigit::infrastructure::database {

/**
 * @brief MySQL实现的ObjectRepository
 * 负责将Git对象（Blob、Commit、Tree）存储到MySQL数据库
 */
class MySQLObjectRepository : public domain::storage::repositories::ObjectRepository {
public:
    MySQLObjectRepository();
    
    bool StoreBlob(const domain::core::Blob& blob) override;
    bool StoreCommit(const domain::core::Commit& commit) override;
    bool StoreTree(const domain::core::Tree& tree) override;
    
    std::unique_ptr<domain::core::Blob> LoadBlob(const std::string& sha1) override;
    std::unique_ptr<domain::core::Commit> LoadCommit(const std::string& sha1) override;
    std::unique_ptr<domain::core::Tree> LoadTree(const std::string& sha1) override;
    
    bool Exists(const std::string& sha1) override;
    std::vector<std::string> GetAllObjectHashes() override;

private:
    /**
     * @brief 通用存储方法
     */
    bool StoreObject(const std::string& sha1, const std::string& type, 
                    const std::vector<uint8_t>& data);
    
    /**
     * @brief 从数据库加载对象数据
     */
    std::vector<uint8_t> LoadObjectData(const std::string& sha1);
};

} // namespace minigit::infrastructure::database
