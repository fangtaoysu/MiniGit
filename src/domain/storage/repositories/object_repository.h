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
    virtual ~ObjectRepository() = default;
    
    virtual bool StoreBlob(const minigit::domain::core::Blob& blob) = 0;
    virtual bool StoreCommit(const minigit::domain::core::Commit& commit) = 0;
    virtual bool StoreTree(const minigit::domain::core::Tree& tree) = 0;
    
    virtual std::unique_ptr<minigit::domain::core::Blob> LoadBlob(const std::string& sha1) = 0;
    virtual std::unique_ptr<minigit::domain::core::Commit> LoadCommit(const std::string& sha1) = 0;
    virtual std::unique_ptr<minigit::domain::core::Tree> LoadTree(const std::string& sha1) = 0;
    
    virtual bool Exists(const std::string& sha1) = 0;
    virtual std::vector<std::string> GetAllObjectHashes() = 0;
};

}  // namespace minigit::domain::storage::repositories