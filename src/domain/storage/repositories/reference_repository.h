#pragma once

#include <string>

namespace minigit::domain::storage::repositories {

class ReferenceRepository {
public:
    virtual ~ReferenceRepository() = default;
    
    virtual bool UpdateHead(const std::string& commit_sha1) = 0;
    virtual std::string GetHead() = 0;
    
    virtual bool CreateBranch(const std::string& branch_name, const std::string& commit_sha1) = 0;
    virtual bool DeleteBranch(const std::string& branch_name) = 0;
    virtual std::string GetBranchHead(const std::string& branch_name) = 0;
    
    virtual bool CreateTag(const std::string& tag_name, const std::string& commit_sha1) = 0;
    virtual std::string ResolveReference(const std::string& ref_name) = 0;
};

} // namespace minigit::domain::storage::repositories