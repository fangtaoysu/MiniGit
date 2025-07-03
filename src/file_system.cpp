#include "../include/file_system.h"


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
    // 1. 检查路径是否存在且是普通文件
    std::filesystem::path path(file_path);
    if (!std::filesystem::exists(path)) {
        return false;
    }

    // 2. 如果是目录直接返回false
    if (std::filesystem::is_directory(path)) {
        return false;
    }

    // 3. UTF-8内容验证
    std::ifstream file(file_path, std::ios::binary);
    if (!file) return false;

    char buffer[1024];
    int expected_bytes = 0; // 用于UTF-8多字节字符追踪
    
    while (file.read(buffer, sizeof(buffer))) {
        for (int i = 0; i < file.gcount(); ++i) {
            unsigned char c = buffer[i];
            
            if (expected_bytes > 0) {
                // 检查UTF-8后续字节格式 (10xxxxxx)
                if ((c & 0xC0) != 0x80) return false;
                expected_bytes--;
            } else {
                // 单字节ASCII (0xxxxxxx)
                if (c <= 0x7F) continue;
                
                // 多字节UTF-8首字节
                if ((c & 0xE0) == 0xC0) expected_bytes = 1;  // 2字节
                else if ((c & 0xF0) == 0xE0) expected_bytes = 2; // 3字节
                else if ((c & 0xF8) == 0xF0) expected_bytes = 3; // 4字节
                else return false; // 非法UTF-8起始字节
            }
        }
    }
    return expected_bytes == 0; // 确保没有截断的多字节字符
}