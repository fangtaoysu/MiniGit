#include "../include/file_system.h"
#include <iostream>


FileSystem::FileSystem(const std::string& file_path)
    : file_path_(file_path) {
    
}

std::stringstream& FileSystem::read_file() const {

}

/** 获取文件大小 */
size_t FileSystem::file_size(const std::string& file_path) {
    try {
        return std::filesystem::file_size(file_path);
    } catch (...) {
        return 0;
    }
}

/**
 * 判断文件是否为有效的UTF-8文本文件
 * @param file_path 文件路径
 * @return true-是UTF-8文本文件，false-不是文本文件或读取失败
 */
bool FileSystem::is_text_file(const std::string& file_path) {
    namespace fs = std::filesystem;
    fs::path path(file_path);

    if (!fs::exists(path) || fs::is_directory(path)) return false;

    std::ifstream file(file_path, std::ios::binary);
    if (!file) return false;

    const size_t max_check_size = 8000;
    char buffer[max_check_size];
    file.read(buffer, sizeof(buffer));
    std::streamsize bytes_read = file.gcount();

    if (bytes_read == 0) return false;

    int nontext_count = 0;

    for (std::streamsize i = 0; i < bytes_read; ++i) {
        unsigned char c = buffer[i];
        if (c == 0) return false; // null 字节直接返回 false
        if (c < 0x09) { // ASCII 控制字符（除了 tab）也视为二进制痕迹
            ++nontext_count;
        }
    }

    // 如果非文本字符占比大于 5%，认为是非文本文件
    double ratio = static_cast<double>(nontext_count) / bytes_read;
    return ratio < 0.05;
}

/**追加文件内容到末尾 */
bool FileSystem::append_file_content(const std::string& file_path, const std::string& append_content) {
    namespace fs = std::filesystem;

    fs::path path(file_path);
    fs::path parent = path.parent_path();

    try {
        // 父目录不存在，则递归创建
        if (!parent.empty() && !fs::exists(parent)) {
            fs::create_directories(parent);
        }

        std::ofstream ofs(file_path, std::ios::app);
        if (!ofs) {
            std::cerr << "打开文件失败：" << file_path << "\n";
            return false;
        }
        
        ofs << append_content;
        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "文件系统错误：" << e.what() << "\n";
        return false;
    }
}