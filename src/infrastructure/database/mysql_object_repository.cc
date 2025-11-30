#include "infrastructure/database/mysql_object_repository.h"

#include "domain/core/blob.h"
#include "domain/core/commit.h"
#include "domain/core/tree.h"
#include "infrastructure/database/database_manager.h"
#include "infrastructure/logging/logger.h"

namespace minigit::infrastructure::database {

MySQLObjectRepository::MySQLObjectRepository() {
}

bool MySQLObjectRepository::StoreBlob(const domain::core::Blob& blob) {
    std::string sha1 = blob.GetSha1();
    
    // 检查是否已存在
    if (Exists(sha1)) {
        LOG_DEBUG("Blob already exists: " << sha1);
        return true;
    }
    
    // 序列化Blob
    auto serialized_data = blob.Serialize();
    std::vector<uint8_t> data(serialized_data.begin(), serialized_data.end());
    
    return StoreObject(sha1, "blob", data);
}

bool MySQLObjectRepository::StoreCommit(const domain::core::Commit& commit) {
    std::string sha1 = commit.GetSha1();
    
    // 检查是否已存在
    if (Exists(sha1)) {
        LOG_DEBUG("Commit already exists: " << sha1);
        return true;
    }
    
    // 序列化Commit
    std::vector<uint8_t> data = commit.Serialize();
    
    return StoreObject(sha1, "commit", data);
}

bool MySQLObjectRepository::StoreTree(const domain::core::Tree& tree) {
    std::string sha1 = tree.GetSha1();
    
    // 检查是否已存在
    if (Exists(sha1)) {
        LOG_DEBUG("Tree already exists: " << sha1);
        return true;
    }
    
    // 序列化Tree
    std::vector<uint8_t> data = tree.Serialize();
    
    return StoreObject(sha1, "tree", data);
}

std::unique_ptr<domain::core::Blob> MySQLObjectRepository::LoadBlob(const std::string& sha1) {
    std::vector<uint8_t> data = LoadObjectData(sha1);
    if (data.empty()) {
        return nullptr;
    }
    
    std::span<const uint8_t> data_span(data);
    domain::core::Blob blob = domain::core::Blob::Deserialize(data_span);
    
    return std::make_unique<domain::core::Blob>(blob);
}

std::unique_ptr<domain::core::Commit> MySQLObjectRepository::LoadCommit(const std::string& sha1) {
    std::vector<uint8_t> data = LoadObjectData(sha1);
    if (data.empty()) {
        return nullptr;
    }
    
    std::span<const uint8_t> data_span(data);
    domain::core::Commit commit = domain::core::Commit::Deserialize(data_span);
    
    return std::make_unique<domain::core::Commit>(commit);
}

std::unique_ptr<domain::core::Tree> MySQLObjectRepository::LoadTree(const std::string& sha1) {
    std::vector<uint8_t> data = LoadObjectData(sha1);
    if (data.empty()) {
        return nullptr;
    }
    
    std::span<const uint8_t> data_span(data);
    std::string tree_sha1 = domain::core::Tree::Deserialize(data_span);
    
    // 重新加载Tree以获取完整对象
    // 注意：这里需要重新构建Tree对象，因为Deserialize只返回SHA1
    // 实际实现中可能需要改进
    return nullptr; // TODO: 需要改进Tree的反序列化
}

bool MySQLObjectRepository::Exists(const std::string& sha1) {
    std::string sql = "SELECT COUNT(*) as count FROM git_objects WHERE sha1 = ?";
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql, {sha1});
    
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

std::vector<std::string> MySQLObjectRepository::GetAllObjectHashes() {
    std::string sql = "SELECT sha1 FROM git_objects";
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql);
    
    std::vector<std::string> hashes;
    for (const auto& row : results) {
        auto it = row.find("sha1");
        if (it != row.end()) {
            hashes.push_back(it->second);
        }
    }
    
    return hashes;
}

bool MySQLObjectRepository::StoreObject(const std::string& sha1, 
                                       const std::string& type, 
                                       const std::vector<uint8_t>& data) {
    std::string sql = R"(
        INSERT INTO git_objects (sha1, type, data, size) 
        VALUES (?, ?, ?, ?)
        ON DUPLICATE KEY UPDATE sha1=sha1
    )";
    
    auto& db_manager = DbManager::GetInstance();
    
    // 将vector<uint8_t>转换为字符串用于存储
    // MySQL的BLOB类型需要特殊处理
    std::string data_str(reinterpret_cast<const char*>(data.data()), data.size());
    
    int64_t result = db_manager.Execute(sql, {sha1, type, data_str, std::to_string(data.size())});
    
    if (result < 0) {
        LOG_ERROR("Failed to store object: " << sha1);
        return false;
    }
    
    LOG_DEBUG("Stored object: " << sha1 << " type: " << type);
    return true;
}

std::vector<uint8_t> MySQLObjectRepository::LoadObjectData(const std::string& sha1) {
    std::string sql = "SELECT data FROM git_objects WHERE sha1 = ?";
    auto& db_manager = DbManager::GetInstance();
    auto results = db_manager.Query(sql, {sha1});
    
    if (results.empty()) {
        return {};
    }
    
    const auto& row = results[0];
    auto it = row.find("data");
    if (it == row.end()) {
        return {};
    }
    
    const std::string& data_str = it->second;
    std::vector<uint8_t> data(data_str.begin(), data_str.end());
    
    return data;
}

} // namespace minigit::infrastructure::database
