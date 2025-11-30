#pragma once

#include <string>  
#include <unordered_map>

namespace minigit::domain::storage::repositories {

class IndexRepository {
public:
    virtual ~IndexRepository() = default;
    
    virtual void StageFile(const std::string& file_path, const std::string& blob_sha1) = 0;
    virtual void UnstageFile(const std::string& file_path) = 0;
    virtual bool IsStaged(const std::string& file_path) = 0;
    virtual std::unordered_map<std::string, std::string> GetStagedFiles() = 0;
    virtual void ClearStaged() = 0;
};


} // namespace domain::storage::repositories