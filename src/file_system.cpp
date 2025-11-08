#include "../include/file_system.h"
#include <iostream>
#include <chrono>
#include <sstream>



std::string FileSystem::read_file(const fs::path& file_path) {
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
    
    return ss.str();
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

/**获取 .mgit目录下所有层级的文件路径 */
std::vector<fs::path> FileSystem::get_all_files(const std::string& mgit_path) {
    std::vector<fs::path> files;
    
    try {
        fs::path mgit_abs = fs::absolute(fs::path(mgit_path) / ".mgit").lexically_normal();
        fs::path parent_dir = mgit_abs.parent_path();
        
        if (!fs::exists(parent_dir)) {
            return files;
        }
        
        for (const auto& entry : fs::recursive_directory_iterator(parent_dir)) {        
            fs::path abs_path = fs::absolute(entry.path()).lexically_normal();
            
            files.push_back(abs_path);
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    
    return files;
}

/**把文件src的内容复制到文件dst中
 * src和dst必须都是绝对路径
 */
bool FileSystem::copy_file_to(const fs::path& src, const fs::path& dst) {
    try {
        if (!fs::exists(dst)) {
            fs::create_directories(dst.parent_path());
            // 创建空文件
            std::ofstream file(dst);
            if (!file) {
                throw std::runtime_error("无法创建文件：" + dst.string());
            }
        }
        
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

    // 将 file_time_type 转为 system_clock::time_point
    using namespace std::chrono;

    auto sctp = time_point_cast<system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + system_clock::now()
    );

    return duration_cast<seconds>(sctp.time_since_epoch()).count();
}