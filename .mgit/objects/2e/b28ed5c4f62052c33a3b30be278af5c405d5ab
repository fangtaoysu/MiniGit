#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <string>
#include <sstream>

struct IndexEntry {
    uint32_t ctime_s;
    uint32_t ctime_n;
    uint32_t mtime_s;
    uint32_t mtime_n;
    uint32_t dev;
    uint32_t ino;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t file_size;
    std::string sha1;
    uint16_t flags;
    std::string path;
};

// 读取 uint32_t（大端）
uint32_t read_uint32_be(std::ifstream &in) {
    unsigned char bytes[4];
    in.read(reinterpret_cast<char *>(bytes), 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// 读取 uint16_t（大端）
uint16_t read_uint16_be(std::ifstream &in) {
    unsigned char bytes[2];
    in.read(reinterpret_cast<char *>(bytes), 2);
    return (bytes[0] << 8) | bytes[1];
}

// 读取 SHA1（20字节转十六进制字符串）
std::string read_sha1(std::ifstream &in) {
    unsigned char sha1[20];
    in.read(reinterpret_cast<char *>(sha1), 20);
    std::ostringstream oss;
    for (int i = 0; i < 20; ++i)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)sha1[i];
    return oss.str();
}

std::vector<IndexEntry> read_git_index(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<IndexEntry> entries;

    if (!file) {
        std::cerr << "Failed to open index file." << std::endl;
        return entries;
    }

    // 读取 header
    char signature[4];
    file.read(signature, 4);
    if (std::string(signature, 4) != "DIRC") {
        std::cerr << "Invalid Git index file (bad signature)." << std::endl;
        return entries;
    }

    uint32_t version = read_uint32_be(file);
    uint32_t entry_count = read_uint32_be(file);
    std::cout << "Git index version: " << version << ", entries: " << entry_count << std::endl;

    for (uint32_t i = 0; i < entry_count; ++i) {
        IndexEntry e;
        e.ctime_s = read_uint32_be(file);
        e.ctime_n = read_uint32_be(file);
        e.mtime_s = read_uint32_be(file);
        e.mtime_n = read_uint32_be(file);
        e.dev = read_uint32_be(file);
        e.ino = read_uint32_be(file);
        e.mode = read_uint32_be(file);
        e.uid = read_uint32_be(file);
        e.gid = read_uint32_be(file);
        e.file_size = read_uint32_be(file);
        e.sha1 = read_sha1(file);
        e.flags = read_uint16_be(file);

        // 路径名（null 结尾字符串）
        std::string path;
        char c;
        while (file.get(c) && c != '\0')
            path += c;
        e.path = path;

        // Git 规定每个 entry 补齐为 8 字节对齐
        size_t entry_len = 62 + path.length() + 1; // 62字节固定 + 路径 + null
        size_t padding = (8 - (entry_len % 8)) % 8;
        file.seekg(padding, std::ios::cur);

        entries.push_back(e);
    }

    return entries;
}

int main() {
    std::string index_path = "../.git/index";
    auto entries = read_git_index(index_path);

    for (const auto &e : entries) {
        std::cout << "📄 File: " << e.path << "\n";
        std::cout << "    SHA1:  " << e.sha1 << "\n";
        std::cout << "    Size:  " << e.file_size << " bytes\n";
        std::cout << "    MTime: " << e.mtime_s << "\n";
        std::cout << std::endl;
    }

    return 0;
}
