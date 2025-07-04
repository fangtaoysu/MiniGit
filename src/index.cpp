#include "../include/index.h"
#include "../include/file_system.h"
#include "../include/utils.h"
#include <fstream>
#include <filesystem>


namespace fs = std::filesystem;
Index::Index(const std::string& mgit_path)
 : mgit_path_(mgit_path), index_path_(mgit_path + "/index") {
}

/**将 .mgit/index 文件读到json中 */
json Index::read_index() const {
    json index = {};
    fs::path index_parent_path = index_path_.parent_path();
    if (!index_parent_path.empty() && !fs::exists(index_parent_path)) {
        return index;
    }
    std::ifstream(index_path_) >> index;
    return index;
}

/**将Json文件写入 .mgit/index 中*/
void Index::write_to_index(json file_info) const {
    fs::path index_parent_path = index_path_.parent_path();

    if (!index_parent_path.empty() && !fs::exists(index_parent_path)) {
        fs::create_directories(index_parent_path);
    }
    std::ofstream(index_path_) << file_info.dump(2);
}

void Index::add() const {
    // 先写死，后面搞成配置文件
    const std::string object_path = mgit_path_ + "/object";
    // 1. 读原有的index文件，如果index为空，那么构造一个空字典
    json index_info = read_index();
    // 2. 遍历所有文件
    std::vector<fs::path> files = FileSystem::get_all_files();
    std::string hash_value;
    
    for (auto & file : files) {
        // 2.1 判断所有类型
        if (FileSystem::is_text_file(file)) {
            // 如果是文本文件，读取内容并根据内容生成哈希
            std::stringstream contents = FileSystem::read_file(file);
            hash_value = Utils::get_hash(contents.str());
        } else {
            // 否则根据文件记录生成哈希
            hash_value = Utils::get_hash(file.string());
        }
        // 2.2 得到哈希后把文件保存在对应哈希所指的路径（./object/）
        const std::string file_path_start = std::string(hash_value.begin(), hash_value.begin()+2);
        const std::string file_path_end = std::string(hash_value.begin()+2, hash_value.end());
        const fs::path dst_path = FileSystem::build_path(object_path, file_path_start, file_path_end);
        FileSystem::copy_file_to(file, dst_path);
        int64_t size =  FileSystem::get_file_size(file);
        int64_t modified = FileSystem::get_file_timestamp(file);
        // 2.3 得到这个文件的其他信息
        index_info[hash_value] = {
            {"path", dst_path},
            {"size", size},
            {"modified", modified}
        };
    }
    // 并写回到这个字典
    write_to_index(index_info);
}
