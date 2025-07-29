#ifndef __UTILS_H__
#define __UTILS_H__
#include <string>
#include <vector>
#include <filesystem>
#include "../thrid_party/nlohmann/json.hpp"


namespace fs = std::filesystem;
using json = nlohmann::json;

struct Utils {
    // 返回值的const：调用者不能修改返回的字符串
    // 末尾的 const: 常量成员函数，函数不能修改类的成员变量
    static const std::string get_current_timestamp();
    static const std::string get_hash(const std::string& hash_source);
    static std::vector<std::string> tokenize(const std::string& input);
    static fs::path generate_obj_path(const std::string& project_path, const std::string& hash);
    static bool is_subpath(const fs::path& parent, const fs::path& child);
    static std::string get_project_path();
};

#endif