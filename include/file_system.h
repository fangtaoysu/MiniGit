/** 提供文件的原子操作
 *  - 文件指纹生成：快速判断文件是否被修改
 *  - 目录遍历：git add 需要用到
 *  - 文件内容读
 *  - 比较两个文件是否相同：git status 用到
 */
#ifndef __FILE_SYSTEM__
#define __FILE_SYSTEM__
#include <fstream>
#include <filesystem>
#include <vector>
#include <utility>


namespace fs = std::filesystem;
class FileSystem {
    public:
        FileSystem(const std::string& file_path);
        static std::stringstream read_file(const fs::path& file_path);
        static size_t file_size(const std::string& file_path);
        static bool is_text_file(const std::string& file_path);
        static bool append_file_content(const std::string& file_path, const std::string& append_content);
        static std::vector<fs::path> get_all_files();
        template <typename... Args>
        static fs::path build_path(Args&&... args);
        static bool copy_file_to(const fs::path& src, const fs::path& dst);
        static int64_t get_file_size(const fs::path& file_path);
        static int64_t get_file_timestamp(const fs::path& file_path);

    private:
        const std::string& file_path_;
        bool static is_inside_excluded(const fs::path& path, const fs::path& exclude_dir); 
};

/**将多个字符串组合成文件路径 */
template <typename... Args>
fs::path FileSystem::build_path(Args&&... args) {
    fs::path result;
    ((result /= std::forward<Args>(args)), ...);
    return result;
}

#endif