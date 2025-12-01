#pragma once

#include <string>
#include <memory>
#include <vector>

#include "domain/core/blob.h"
#include "domain/core/commit.h"
#include "domain/core/tree.h"

namespace minigit::domain::storage::repositories {

class ObjectRepository {
public:    
    bool StoreBlob(const minigit::domain::core::Blob& blob);
    bool StoreCommit(const minigit::domain::core::Commit& commit);
    bool StoreTree(const minigit::domain::core::Tree& tree);
    
    std::unique_ptr<minigit::domain::core::Blob> LoadBlob(const std::string& sha1);
    std::unique_ptr<minigit::domain::core::Commit> LoadCommit(const std::string& sha1);
    std::unique_ptr<minigit::domain::core::Tree> LoadTree(const std::string& sha1);
    
    bool Exists(const std::string& sha1);
    std::vector<std::string> GetAllObjectHashes();
};

}  // namespace minigit::domain::storage::repositories