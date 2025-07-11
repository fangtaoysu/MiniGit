#include "../include/index.h"
#include "../include/file_system.h"
#include "../include/utils.h"
#include "../include/object_db.h"
#include <fstream>
#include <filesystem>



namespace fs = std::filesystem;
Index::Index(const std::string& project_path)
 : project_path_(project_path), index_path_(project_path + "/.mgit/index") {
}

/**将 .mgit/index 文件读到json中 */
json Index::get_index() const {
    json index = {};
    if (!fs::exists(index_path_)) {
        return index;
    }
    std::ifstream(index_path_) >> index;
    return index;
}

void Index::reset_index_entries() {
    json index = get_index();
    index["entries"]["counts"] = 0;
    index["entries"]["create_files"].clear();
    write_to_index(index);
}

/**将Json文件写入 .mgit/index 中*/
void Index::write_to_index(json file_info) const {
    fs::path index_parent_path = index_path_.parent_path();

    if (!index_parent_path.empty() && !fs::exists(index_parent_path)) {
        fs::create_directories(index_parent_path);
    }
    std::ofstream(index_path_) << file_info.dump(2);
}

void Index::add(const std::vector<fs::path>* files) const {
    ObjectDB db(project_path_);
    // 1. 读原有的index文件，如果index为空，那么构造一个空字典
    json index_info = get_index();

    // 2. 得到需要add的文件
    std::vector<fs::path> all_files;
    if (!files) {
        all_files = FileSystem::get_all_files(project_path_);
        files = &all_files;
    }
    std::string hash_value;\
    size_t count = 0;
    std::vector<std::string> create_files;

    for (auto & file : *files) {
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
        const fs::path& dst_path = db.write(hash_value, file);
        int64_t size =  FileSystem::get_file_size(file);
        int64_t modified = FileSystem::get_file_timestamp(file);
        // 判断是否是修改的文件，是否是新增的文件
        auto it = index_info.find(dst_path);
        if (it == index_info.end()) { // 如果当前路径不在其中，说明是新建的文件
            create_files.push_back(file);
            count += 1;
        } else { 
            // 如果是修改的的文件
            if (index_info[dst_path]["hash"] != hash_value) {
                count += 1;
            }
        }
        // 2.3 得到这个文件的其他信息
        index_info[dst_path] = {
            {"hash", hash_value},
            {"size", size},
            {"modified", modified}
        };
    }
    index_info["entries"] = {
        {"counts", count}, // 有变化文件的数量
        {"create_files", create_files}
    };
    // 并写回到这个字典
    write_to_index(index_info);
}
