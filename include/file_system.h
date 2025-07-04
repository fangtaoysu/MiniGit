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


class FileSystem {
    public:
        FileSystem(const std::string& file_path);
        std::stringstream& read_file() const;
        size_t static file_size(const std::string& file_path);
        bool static is_text_file(const std::string& file_path);
        bool static append_file_content(const std::string& file_path, const std::string& append_content);

    private:
        const std::string& file_path_;
};

#endif