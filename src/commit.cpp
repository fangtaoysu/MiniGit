#include "../include/commit.h"
#include <openssl/sha.h> // sha-1 哈希
#include <iomanip>
#include <chrono>


/**
 * 创建一个新的提交对象
 */

Commit::Commit (const std::string& msg, const std::string& father_ref, std::unordered_map<std::string, std::string> changed_files)
    : msg_(msg),
    // const 成员变量必须在 构造函数的初始化列表（initializer list） 中初始化
      father_ref_(father_ref.empty() ? std::string(40, '0') : father_ref) {
    // 调用生成hash的函数
    const std::string& current_ref = get_hash();
    // 自动设置时间戳
    const std::string& current_timestamp = get_current_timestamp();
    // streamsteam流需要分隔符
    // 长度前缀的方式进行序列化存储
    commit_item_ << father_ref_.length() << " " << father_ref_
                 << current_ref.length() << " " << current_ref
                 << current_timestamp.length() << " " << current_timestamp
                 << msg.length() << " " << msg;
}

const std::stringstream& Commit::get_commit_item() const {
    return commit_item_;
}

/**
 * 根据commit的消息生成哈希值(20位)
 */
const std::string Commit::get_hash() const {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(msg_.c_str()), msg_.size(), hash);
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