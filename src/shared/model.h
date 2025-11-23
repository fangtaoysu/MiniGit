#pragma once
#include <string>

struct LoggingSettings {
    std::string level{"info"};
};

struct MySqlSettings {
    bool enable{false};
    std::string host{"127.0.0.1"};
    int port{3306};
    std::string user{"root"};
    std::string password{};
    std::string db_name{""};
    int pool_size{0};
};

struct ThreadPoolSettings {
    int size{0};
};

struct LexicalResult {
    std::string command;                    // 命令主体: init, add, commit
    std::vector<std::string> option;        // 所有选项: -m, --message, -v, --verbose  
    std::vector<std::string> argument;      // 普通参数: 消息内容, 分支名
    std::vector<std::string> file_path;     // 文件路径
};