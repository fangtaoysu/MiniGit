#ifndef __OBJECT_DB_H__
#define __OBJECT_DB_H__

#include <string>
#include <vector>
#include <filesystem>
#include "../thrid_party/nlohmann//json.hpp"
#include "utils.h"


template<typename> constexpr bool always_false = false;

using json = nlohmann::json;
namespace fs = std::filesystem;
class ObjectDB {
    public:
        ObjectDB()=default;
        // 将内容根据对象数据库规则写入指定路径
        template <typename content_type>
        const fs::path write(const std::string& hash, const content_type& write_contents);

        template <typename return_type>
        return_type read(const std::string& hash);

    private:
        const std::string project_path_ = Utils::get_project_path();
        bool write_path(const fs::path& src_path, const fs::path& des_path);
        bool write_json(const json& write_contents, const fs::path& des_path);
        bool write_lines(const std::vector<std::string>& write_contents, const fs::path& des_path);

        json read_json(const fs::path& file_path);
        std::vector<std::string> read_lines(const fs::path& file_path);
};


template <typename content_type>
const fs::path ObjectDB::write(const std::string& hash, const content_type& write_contents) {
    const fs::path obj_path = Utils::generate_obj_path(project_path_, hash);

    // 根据类型选择写入方式
    if constexpr (std::is_same_v<content_type, fs::path>) {
        write_path(write_contents, obj_path);
    } else if constexpr (std::is_same_v<content_type, json>) {
        write_json(write_contents, obj_path);
    } else if constexpr (std::is_same_v<content_type, std::vector<std::string>>) {
        write_lines(write_contents, obj_path);
    } else {
        static_assert(always_false<content_type>, "Unsupported content type");
        return "";
    }
    return obj_path;
}

template <typename return_type>
return_type ObjectDB::read(const std::string& hash) {
    const fs::path obj_path = Utils::generate_obj_path(project_path_, hash);
    
    // 根据返回值类型选择读取方式,is_same_v判断类型是否相同
    if constexpr (std::is_same_v<return_type, json>) {
        return read_json(obj_path);
    } else if constexpr (std::is_same_v<return_type, std::vector<std::string>>) {
        return read_lines(obj_path);
    } else {
        static_assert(always_false<return_type>, "Unspoorted return type");
    }
}

#endif