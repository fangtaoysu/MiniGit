#pragma once

namespace minigit::presentation {

struct LexicalResult {
    std::string command;              // 命令主体: init, add, commit
    std::vector<std::string> option;  // 所有选项: -m, --message, -v, --verbose
    std::vector<std::string> argument;  // 普通参数: 消息内容, 分支名
    std::vector<std::string> file_path;  // 文件路径
};
}  // namespace minigit::presentation