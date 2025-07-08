

#include "../include/commit.h"
#include "../include/file_system.h"
#include "../include/utils.h"
#include "../include/object_db.h"
#include "../include/config.h"
#include <iostream>



/**
 * 创建一个新的提交对象
 */
Commit::Commit (const std::string& project_path)
    : project_path_(project_path), current_ref_(""), current_timestamp_("") {
}

const std::string Commit::get_father_ref() {
    fs::path HEAD_path = fs::path(project_path_) / ".mgit" / "logs" / "HEAD";
    std::cout << HEAD_path << std::endl;
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

/**将本条commit对象保存在`.mgit/logs/HEAD`中 */
const std::string Commit::run(const std::string& msg, std::string father_ref) {
    // 检查父节点
    father_ref = get_father_ref();
    // 打上当前的时间戳
    current_timestamp_ = Utils::get_current_timestamp();
    // 生成本条commit的哈希
    const std::string hash_source = msg + current_timestamp_;
    current_ref_ = Utils::get_hash(hash_source);
    // 将完整的commit对象保存到 .gmit/objects 中
    save_to_objects(father_ref, msg);
    save_to_HEAD(father_ref, msg);
    
    return current_ref_;
}

/** 将这条commit相关的信息存入object数据库 */
void Commit::save_to_objects(const std::string& father_ref, const std::string& msg) {
    Config config(project_path_);
    ObjectDB db(project_path_);
    std::string author(
        config.user_.name + " <" + config.user_.email + "> " + current_timestamp_
    );
    json write_contents = {
        {"parent", father_ref},
        {"author", author},
        {"commit", msg}
    };
    db.write(current_ref_, write_contents);
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