#pragma once

#include <string>
#include <vector>

namespace minigit::presentation {

struct CommandContext {
    std::string cmd;                // 命令主体: init, add, commit
    std::vector<std::string> opts;  // 所有选项: -m, --message, -v, --verbose
    std::vector<std::string> args;  // 普通参数: 消息内容, 分支名
    std::vector<std::string> file_paths;  // 文件路径
};

}  // namespace minigit::presentation