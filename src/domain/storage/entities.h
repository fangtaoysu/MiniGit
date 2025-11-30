#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace domain::storage {

struct GitObjectEntity {
    std::string sha1;
    std::string type;  // "blob", "commit", "tree"
    std::vector<uint8_t> data;
    size_t size;
    std::chrono::system_clock::time_point created_at;
};

struct ReferenceEntity {
    std::string name;     // "HEAD", "refs/heads/main", "refs/tags/v1.0"
    std::string target;   // commit SHA1 or other reference
    std::string type;     // "branch", "tag", "head"
};

struct IndexEntryEntity {
    std::string file_path;
    std::string blob_sha1;
    std::string mode;     // "100644", "100755", "040000"
    std::chrono::system_clock::time_point staged_at;
};

} // namespace domain::storage