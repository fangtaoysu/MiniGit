#ifndef __UTILS_H__
#define __UTILS_H__
#include <string>
#include <vector>
#include <filesystem>


namespace fs = std::filesystem;

class Utils {
    public:
        // 返回值的const：调用者不能修改返回的字符串
        // 末尾的 const: 常量成员函数，函数不能修改类的成员变量
        static const std::string get_current_timestamp();
        static const std::string get_hash(const std::string& hash_source);
        static std::vector<std::string> tokenize(const std::string& input);
        static fs::path generate_obj_path(const std::string& project_path, const std::string& hash);

    private:

};

#endif