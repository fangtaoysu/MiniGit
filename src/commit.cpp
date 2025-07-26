

#include "../include/commit.h"
#include "../include/file_system.h"
#include "../include/utils.h"
#include "../include/object_db.h"
#include "../include/config.h"
#include "index.h"
#include <fstream>
#include <iostream>



/** 创建一个新的提交对象 */
Commit::Commit (const std::string& project_path)
    : project_path_(project_path), current_ref_("") {
}


const std::string Commit::read_commit_hash(const std::string& project_path) {
    fs::path HEAD_path = fs::path(project_path) / ".mgit" / "logs" / "HEAD";
    const std::string father_ref(40, '0');
    if (!fs::exists(HEAD_path)) {
        return father_ref;
    }

    // 读最后一行
    std::ifstream file(HEAD_path);
    std::string line, last_line;
    while (std::getline(file, line)) {
        last_line = line;
    }

    // 读最后一行的哈希（上一个commit的哈希）作为当前的父哈希
    size_t first = last_line.find(' ');
    size_t second = last_line.find(' ', first+1);
    return last_line.substr(first + 1, second - first - 1);
}


/** 将本条commit对象保存在`.mgit/logs/HEAD`中 */
void Commit::run(const std::string& msg) {
    Index index_object(project_path_);
    json index = index_object.get_index();

    // 打上当前的时间戳
    current_timestamp_ = Utils::get_current_timestamp();

    // 检查暂存区是否有内容
    if (!is_index_changed(index.at("entries"))) {
        return;
    }

    // 筛选本次commit对应的tree对象
    std::vector<std::string> tree_objects;
    for (auto& [key, value] : index.items()) {
        if (value.contains("is_commit") && !value["is_commit"]) {
            tree_objects.push_back(key);
        }
    }
    // 上一个commit哈希作为本次commit对象的父节点
    const std::string father_ref = Commit::read_commit_hash(project_path_);

    // 生成本条commit的哈希
    const std::string hash_source = msg + current_timestamp_;
    current_ref_ = Utils::get_hash(hash_source);

    // 将完整的commit对象保存到 .gmit/objects 中
    save_to_objects(father_ref, tree_objects, msg);
    save_to_HEAD(father_ref, msg);
    index_object.reset_index_entries();
}


bool Commit::is_index_changed(const json& entries) const {
    if (entries.at("counts") == 0) {
        std::cerr << "no changes added to commit (use \"git add\" and/or \"git commit -a\")\n";
        return false;
    }
    std::cout << " " << entries.at("counts") << " files changed\n";
    for (auto & file_path : entries.at("create_files")) {
        std::cout << " create mode " << file_path << std::endl;
    }
    return true;
}


/** 将commit对象和tree对象均存入object数据库 */
void Commit::save_to_objects(const std::string& father_ref, const std::vector<std::string>& tree_objects, const std::string& msg) {
    // 生成tree对象的哈希
    std::string tree_hash = Utils::get_hash(current_ref_);

    // 构造author
    Config config(project_path_);
    ObjectDB db(project_path_);
    std::string author(
        config.user_.name + " <" + config.user_.email + "> " + current_timestamp_
    );
        
    json commit_object = {
        {"parent", father_ref},
        {"author", author},
        {"commit", msg},
        {"tree", tree_hash}
    };
    db.write(current_ref_, commit_object);
    db.write(tree_hash, tree_objects);
}


/** 将一条commit对象存入HEAD */
void Commit::save_to_HEAD(const std::string& father_ref, const std::string& msg) {
    // 生成存入HEAD的msg
    std::string line_msg;
    for (size_t i = 0; i < msg.size(); ++i) {
        const char c = msg[i];
        if (c == '\n') {
            break;
        }
        line_msg += c;
    }

    // 把get_commit_object追加到文件中
    std::stringstream ss;
    ss << father_ref << " "
       << current_ref_ << " "
       << current_timestamp_ << " "
       << line_msg << "\n";

    const std::string HEAD_path = project_path_ + "/.mgit/logs/HEAD";
    FileSystem::append_file_content(HEAD_path, ss.str());
}

std::vector<std::string> Commit::read_tree_object(const std::string& project_path) {
    std::vector<std::string> tree_objects;
    std::string tree_hash;
    std::string line;
    
    // 读HEAD文件解析出commit哈希
    const std::string commit_hash = read_commit_hash(project_path);
    
    // 读commit对象解析出tree对象
    fs::path commit_obj_path = Utils::generate_obj_path(project_path, commit_hash);
    std::ifstream commit_file(commit_obj_path);
    json commit_obj = json::parse(commit_file);
    if (!commit_obj.contains("tree")) {
        return tree_objects;
    }
    tree_hash = commit_obj["tree"];
    fs::path tree_obj_path = Utils::generate_obj_path(project_path, tree_hash);

    std::ifstream tree_file(tree_obj_path);
    while (std::getline(tree_file, line)) {
        tree_objects.push_back(line);
    }

    return tree_objects;  
}