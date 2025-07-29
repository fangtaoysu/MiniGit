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
        // std::vector<std::string> read(const std::string& hash);
        template <typename content_type>
        const fs::path write(const std::string& hash, const content_type& write_contents);

    private:
        const std::string project_path_ = Utils::get_project_path();
        bool write_path(const fs::path& src_path, const fs::path& des_path);
        bool write_json(const json& write_contents, const fs::path& des_path);
        bool write_lines(const std::vector<std::string>& write_contents, const fs::path& des_path);
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

#endif