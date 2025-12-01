#pragma once

#include <string>

namespace minigit::domain::storage::repositories {

class ReferenceRepository {
public:
    bool UpdateHead(const std::string& commit_sha1);
    std::string GetHead();
    
    bool CreateBranch(const std::string& branch_name, const std::string& commit_sha1);
    bool DeleteBranch(const std::string& branch_name);
    std::string GetBranchHead(const std::string& branch_name);
    
    bool CreateTag(const std::string& tag_name, const std::string& commit_sha1);
    std::string ResolveReference(const std::string& ref_name);
};

} // namespace minigit::domain::storage::repositories