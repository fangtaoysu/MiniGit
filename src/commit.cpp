

#include "../include/commit.h"
#include "../include/file_system.h"
#include "../include/utils.h"

/**
 * 创建一个新的提交对象
 */

Commit::Commit (const std::string& msg, const std::string& father_ref)
    : msg_(msg),
    // const 成员变量必须在 构造函数的初始化列表（initializer list） 中初始化
      father_ref_(father_ref.empty() ? std::string(40, '0') : father_ref) {
    // 自动设置时间戳
    const std::string& current_timestamp = Utils::get_current_timestamp();
    // 调用生成hash的函数
    const std::string hash_source = msg_ + current_timestamp;
    const std::string& current_ref = Utils::get_hash(hash_source);
    // streamsteam流需要分隔符
    // 长度前缀的方式进行序列化存储
    commit_object_.father_ref = father_ref_;
    commit_object_.current_ref = current_ref;
    commit_object_.current_timestamp = current_timestamp;
    commit_object_.msg = msg_;
}

const CommitObject& Commit::get_commit_object() const {
    return commit_object_;
}

/**将本条commit对象保存在`.mgit/logs/HEAD`中 */
void Commit::run() {
    // 把get_commit_object追加到文件中
    std::stringstream ss;
    ss << commit_object_.father_ref << " "
       << commit_object_.current_ref << " "
       << commit_object_.current_timestamp << " "
       << commit_object_.msg << "\n";
    const std::string file_path = "../.mgit/logs/HEAD";
    FileSystem::append_file_content(file_path, ss.str());
}