#pragma once

#include <string>
#include <unordered_map>

namespace minigit::domain::storage::repositories {

// 引用类型（如分支、标签）
enum class ReferenceType { kBranch, kTag, kHead };

class ReferenceRepository {
public:
    ReferenceRepository() = default;
    ~ReferenceRepository() = default;

    // 引用管理
    bool Set(const std::string& ref_name, const std::string& target_hash,
             ReferenceType type = ReferenceType::kBranch);
    std::string Get(const std::string& ref_name) const;
    bool Remove(const std::string& ref_name);
    bool Exists(const std::string& ref_name) const;

private:
    struct Reference {
        std::string target_hash;
        ReferenceType type;
    };
    std::unordered_map<std::string, Reference> references_;
};

}  // namespace minigit::domain::storage::repositories
