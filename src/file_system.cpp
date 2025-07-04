#include "../include/file_system.h"
#include <iostream>
#include <gtest/gtest.h>



FileSystem::FileSystem(const std::string& file_path)
    : file_path_(file_path) {
    
}

std::stringstream FileSystem::read_file(const fs::path& file_path) {
    // 确保静态变量只初始化一次（线程安全）
    std::stringstream ss;
    
    // 每次调用前清空流状态
    ss.str("");
    ss.clear();
    
    // 检查文件是否存在
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("File not found: " + file_path.string());
    }
    
    // 检查是否为普通文件
    if (!std::filesystem::is_regular_file(file_path)) {
        throw std::runtime_error("Not a regular file: " + file_path.string());
    }
    
    // 打开文件并读取内容
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path.string());
    }
    
    // 读取文件内容到 stringstream
    ss << file.rdbuf();
    
    // 检查是否读取成功
    if (file.fail() && !file.eof()) {
        throw std::runtime_error("Error reading file: " + file_path.string());
    }
    
    return ss;
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

/**检查path是否以base开头 */
bool FileSystem::is_inside_excluded(const fs::path& path, const fs::path& exclude_dir) {
    const std::string path_str = fs::absolute(path).lexically_normal().string();
    const std::string excl_str = fs::absolute(exclude_dir).lexically_normal().string();

    // 确保排除目录路径以分隔符结尾
    const char sep = fs::path::preferred_separator;
    const std::string excl_with_sep = excl_str.back() == sep ? excl_str : excl_str + sep;

    return path_str.find(excl_with_sep) == 0;
}

/**获取 .mgit目录下所有层级的文件路径 */
std::vector<fs::path> FileSystem::get_all_files() {
    const std::string& mgit_path = "../.git";
    std::vector<fs::path> files;
    // 建一个临时的build
    fs::path build_abs = fs::absolute("../build");
    
    try {
        fs::path mgit_abs = fs::absolute(mgit_path).lexically_normal();
        fs::path parent_dir = mgit_abs.parent_path();
        
        if (!fs::exists(parent_dir)) {
            return files;
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(parent_dir)) {        
            fs::path abs_path = fs::absolute(entry.path()).lexically_normal();
            fs::path rel_path = fs::relative(abs_path, parent_dir);
            
            if (is_inside_excluded(abs_path, mgit_abs) || is_inside_excluded(abs_path, build_abs) || fs::is_directory(abs_path)) {
                continue;
            }
            files.push_back(rel_path);
        }
    } catch (const fs::filesystem_error& e) {
        GTEST_LOG_(ERROR) << "Filesystem error: " << e.what() << std::endl;
    }
    
    return files;
}

bool FileSystem::copy_file_to(const fs::path& src, const fs::path& dst) {
    try {
        fs::create_directories(dst.parent_path());

        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        return true;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Copy failed: " << e.what() << std::endl;
        return false;
    }
}

/** 获取文件大小 */
int64_t FileSystem::get_file_size(const fs::path& file_path) {
    std::error_code ec;
    auto size = fs::file_size(file_path, ec);
    return ec ? -1 : static_cast<int64_t>(size);
}

/** 获取文件最近一次修改的时间戳 */
int64_t FileSystem::get_file_timestamp(const fs::path& file_path) {
    std::error_code ec;
    if (!fs::exists(file_path, ec) || ec) {
        return -1;
    }
    auto ftime = fs::last_write_time(file_path, ec);
    if (ec) {
        return -1;
    }

    auto sctp = std::chrono::time_point_cast<std::chrono::seconds>(ftime);
    int64_t timestamp = sctp.time_since_epoch().count();

    // 确保是10位时间戳
    const int64_t ten_digits = 1000000000;
    return timestamp % ten_digits;
}