#include "domain/core/tree.h"
#include <openssl/sha.h>

namespace minigit::domain::core {

Tree::Tree(std::vector<TreeEntry> entries)
    : obj_type_("tree"), entries_(std::move(entries)) {
    sha1_ = CalculateSha1();
}

Tree::Tree() : obj_type_("tree"), entries_{} {
    sha1_ = CalculateSha1();
}

Tree Tree::AddEntry(const std::string& mode,
                     const std::string& obj_type, 
                     const std::string& sha1,
                     const std::string& name) const {
    std::vector<TreeEntry> new_entries = entries_;
    new_entries.push_back(TreeEntry{mode, obj_type, sha1, name});
    return Tree(new_entries);
}

std::string Tree::CalculateSha1() const {
    std::string data;
    for (const auto& entry : entries_) {
        data += entry.mode + " " + entry.obj_type + " " + entry.sha1 + "\t" + entry.name + "\n";
    }
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.data()), data.size(), hash);
    static const char* kHex = "0123456789abcdef";
    std::string out;
    out.resize(2 * SHA_DIGEST_LENGTH);
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        out[2 * i] = kHex[(hash[i] >> 4) & 0xF];
        out[2 * i + 1] = kHex[hash[i] & 0xF];
    }
    return out;
}

std::string Tree::GetSha1() const {
    return sha1_;
}

std::string Tree::Deserialize(std::span<const uint8_t> data) {
    std::string s(reinterpret_cast<const char*>(data.data()), data.size());
    std::vector<TreeEntry> entries;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t end = s.find('\n', pos);
        if (end == std::string::npos) end = s.size();
        std::string line = s.substr(pos, end - pos);
        pos = end + 1;
        if (line.empty()) continue;

        size_t tab = line.find('\t');
        if (tab == std::string::npos) continue;
        std::string left = line.substr(0, tab);
        std::string name = line.substr(tab + 1);

        size_t first_space = left.find(' ');
        size_t second_space = left.find(' ', first_space + 1);
        if (first_space == std::string::npos || second_space == std::string::npos) continue;

        std::string mode = left.substr(0, first_space);
        std::string obj_type = left.substr(first_space + 1, second_space - first_space - 1);
        std::string sha1 = left.substr(second_space + 1);

        entries.push_back(TreeEntry{mode, obj_type, sha1, name});
    }
    Tree tree(entries);
    return tree.sha1_;
}

std::vector<uint8_t> Tree::Serialize() const {
    std::string out;
    out.reserve(128);
    for (const auto& e : entries_) {
        out += e.mode + " " + e.obj_type + " " + e.sha1 + "\t" + e.name + "\n";
    }
    return std::vector<uint8_t>(out.begin(), out.end());
}

} // namespace domain::core
