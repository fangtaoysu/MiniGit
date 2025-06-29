#include "../include/commit.h"

/**
 * 创建一个新的提交对象
 */
Commit::Commit (const std::string& msg, const std::string& father_ref, std::unordered_map<std::string, std::string> changed_files) : msg_(msg) {
    // 自动设置时间戳
    
    // 调用生成hash的函数

}

