#pragma once

#include <string>
#include <unordered_map>

namespace minigit::domain::storage::repositories {

class IndexRepository {
public:
    void StageFile(const std::string& file_path, const std::string& blob_sha1);
    void UnstageFile(const std::string& file_path);
    bool IsStaged(const std::string& file_path);
    std::unordered_map<std::string, std::string> GetStagedFiles();
    void ClearStaged();
};


} // namespace domain::storage::repositories