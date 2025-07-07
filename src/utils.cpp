#include "../include/utils.h"
#include <openssl/sha.h> // sha-1 哈希
#include <sstream>
#include <iomanip>
#include <chrono>



/**
 * 根据commit的消息生成哈希值(20位)
 */
const std::string Utils::get_hash(const std::string& hash_source) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    // std::string hash_source = msg_ + get_current_timestamp();
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
const std::string Utils::get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::string timestamp_str = std::to_string(timestamp);
    // 长度不够则补前导0
    if (timestamp_str.length() < 10) {
        timestamp_str = std::string(10 - timestamp_str.length(), '0') + timestamp_str;
    }
    return timestamp_str;
}

/** 分词器 */
std::vector<std::string> Utils::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool in_quote = false;
    char quote_char = '\0';

    for (size_t i = 0; i < input.size(); ++i) {
        const char c = input[i];
        
        // 先收集范围最大的（引号）
        if ((c == '\'' || c == '"') && (!in_quote || c == quote_char)) {
            in_quote = !in_quote;
            quote_char = in_quote ? c : '\0';
            continue;
        }

        // 在引号内保留所有字符（包括空格和换行）
        if (in_quote) {
            current += c;
            continue;
        }

        // 非引号下的分隔符
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            continue;
        }

        // 普通字符
        current += c;
    }

    // 添加最后一个token
    if (!current.empty()) {
        tokens.push_back(current);
    }

    // 验证引号闭合
    if (in_quote) {
        throw std::runtime_error("Unclosed quote in input");
    }

    return tokens;
}