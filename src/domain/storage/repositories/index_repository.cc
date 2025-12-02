#include "domain/storage/repositories/index_repository.h"

#include <chrono>

namespace minigit::domain::storage::repositories {

bool IndexRepository::Add(const std::string& path,
                          const std::string& object_hash) {
    if (path.empty() || object_hash.empty()) {
        return false;
    }

    auto now = std::chrono::system_clock::now();
    entries_[path] =
        IndexEntry{.path = path,
                   .object_hash = object_hash,
                   .timestamp = std::chrono::system_clock::to_time_t(now)};
    return true;
}

bool IndexRepository::Remove(const std::string& path) {
    auto it = entries_.find(path);
    if (it == entries_.end()) {
        return false;
    }
    entries_.erase(it);
    return true;
}

std::string IndexRepository::GetHash(const std::string& path) const {
    auto it = entries_.find(path);
    if (it == entries_.end()) {
        return "";
    }
    return it->second.object_hash;
}

bool IndexRepository::Contains(const std::string& path) const {
    return entries_.find(path) != entries_.end();
}

}  // namespace minigit::domain::storage::repositories