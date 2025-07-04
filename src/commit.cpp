#include <openssl/sha.h> // sha-1 哈希
#include <iomanip>
#include <chrono>
#include "../include/commit.h"
#include "../include/file_system.h"

/**
 * 创建一个新的提交对象
 */

Commit::Commit (const std::string& msg, const std::string& father_ref)
    : msg_(msg),
    // const 成员变量必须在 构造函数的初始化列表（initializer list） 中初始化
      father_ref_(father_ref.empty() ? std::string(40, '0') : father_ref) {
    // 调用生成hash的函数
    const std::string& current_ref = get_hash();
    // 自动设置时间戳
    const std::string& current_timestamp = get_current_timestamp();
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

/**
 * 根据commit的消息生成哈希值(20位)
 */
const std::string Commit::get_hash() const {
    unsigned char hash[SHA_DIGEST_LENGTH];
    std::string hash_source = msg_ + get_current_timestamp();
    SHA1(reinterpret_cast<const unsigned char*>(hash_source.c_str()), hash_source.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<int>(hash[i]);
    }
    return ss.str();
}

/**
 * 获取当前时间戳(10位)
 */
const std::string Commit::get_current_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::string timestamp_str = std::to_string(timestamp);
    // 长度不够则补前导0
    if (timestamp_str.length() < 10) {
        timestamp_str = std::string(10 - timestamp_str.length(), '0') + timestamp_str;
    }
    return timestamp_str;
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