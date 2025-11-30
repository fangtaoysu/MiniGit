#include "domain/core/commit.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

namespace minigit::domain::core {

Commit::Commit(const CommitMetadata& metadata)
    : meta_data_(metadata) {
    meta_data_.tree_sha1 = metadata.tree_sha1;
    meta_data_.parent_commits = metadata.parent_commits;
    meta_data_.author = metadata.author;
    meta_data_.committer = metadata.committer;
    meta_data_.message = metadata.message;
    sha1_ = CalculateSha1();
}

std::string Commit::CalculateSha1() const {
    auto serialized_data = Serialize();
    
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(serialized_data.data(), serialized_data.size(), hash);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::setw(2) << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::vector<uint8_t> Commit::Serialize() const {
    std::stringstream ss;
    
    ss << "tree " << meta_data_.tree_sha1 << "\n";
    
    for (const auto& Parent : meta_data_.parent_commits) {
        ss << "parent " << Parent << "\n";
    }
    
    ss << "author " << meta_data_.author << "\n";
    ss << "committer " << meta_data_.committer << "\n";
    ss << "\n";
    ss << meta_data_.message;
    
    if (!meta_data_.message.empty() && meta_data_.message.back() != '\n') {
        ss << "\n";
    }
    
    std::string Result = ss.str();
    return std::vector<uint8_t>(Result.begin(), Result.end());
}

Commit Commit::Deserialize(std::span<const uint8_t> Data) {
    std::string s(reinterpret_cast<const char*>(Data.data()), Data.size());
    CommitMetadata metadata;
    
    size_t pos = 0;
    while (pos < s.size()) {
        size_t end = s.find('\n', pos);
        if (end == std::string::npos) end = s.size();
        std::string line = s.substr(pos, end - pos);
        pos = end + 1;
        if (line.empty()) break;  // End of headers
        
        if (line.rfind("tree ", 0) == 0) {
            metadata.tree_sha1 = line.substr(5);
        } else if (line.rfind("parent ", 0) == 0) {
            metadata.parent_commits.push_back(line.substr(7));
        } else if (line.rfind("author ", 0) == 0) {
            metadata.author = line.substr(7);
        } else if (line.rfind("committer ", 0) == 0) {
            metadata.committer = line.substr(10);
        }
    }
    
    // The rest is the commit message
    metadata.message = s.substr(pos);
    
    return Commit(metadata);
}

const std::string& Commit::GetSha1() const {
    return sha1_;

}

const std::string& Commit::GetTreeSha1() const {
    return meta_data_.tree_sha1;
}

const std::vector<std::string>& Commit::GetParentCommits() const {
    return meta_data_.parent_commits;
}

const std::string& Commit::GetAuthor() const {
    return meta_data_.author;
}

const std::string& Commit::GetCommitter() const {
    return meta_data_.committer;
}

const std::string& Commit::GetMessage() const {
    return meta_data_.message;
}

} // namespace domain::core