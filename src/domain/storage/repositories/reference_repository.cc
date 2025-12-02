#include "domain/storage/repositories/reference_repository.h"

namespace minigit::domain::storage::repositories {

bool ReferenceRepository::Set(const std::string& ref_name,
                              const std::string& target_hash,
                              ReferenceType type) {
    if (ref_name.empty() || target_hash.empty()) {
        return false;
    }
    references_[ref_name] = Reference{.target_hash = target_hash, .type = type};
    return true;
}

std::string ReferenceRepository::Get(const std::string& ref_name) const {
    auto it = references_.find(ref_name);
    if (it == references_.end()) {
        return "";
    }
    return it->second.target_hash;
}

bool ReferenceRepository::Remove(const std::string& ref_name) {
    return references_.erase(ref_name) > 0;
}

bool ReferenceRepository::Exists(const std::string& ref_name) const {
    return references_.find(ref_name) != references_.end();
}

}  // namespace minigit::domain::storage::repositories