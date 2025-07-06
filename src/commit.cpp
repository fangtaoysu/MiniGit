

#include "../include/commit.h"
#include "../include/file_system.h"
#include "../include/utils.h"

/**
 * 创建一个新的提交对象
 */

Commit::Commit (const std::string& project_path)
    : project_path_(project_path) {
}

/**将本条commit对象保存在`.mgit/logs/HEAD`中 */
const std::string Commit::run(const std::string& msg, std::string father_ref) {
    // 检查父节点
    father_ref = father_ref.empty() ? std::string(40, '0') : father_ref;
     // 打上当前的时间戳
    const std::string& current_timestamp = Utils::get_current_timestamp();
    // 生成本条commit的哈希
    const std::string hash_source = msg + current_timestamp;
    const std::string& current_ref = Utils::get_hash(hash_source);
    // 把get_commit_object追加到文件中
    std::stringstream ss;
    ss << father_ref << " "
       << current_ref << " "
       << current_timestamp << " "
       << msg << "\n";
    const std::string HEAD_path = project_path_ + "/.mgit/logs/HEAD";
    FileSystem::append_file_content(HEAD_path, ss.str());
    return current_ref;
}