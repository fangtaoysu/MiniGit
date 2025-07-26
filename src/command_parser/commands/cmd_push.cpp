#include "../../../include/command_parser/commands/cmd_push.h"
#include <iostream>


void CmdPush::execute(const ParsedCommand& cmd, Repository& repo) {
    // 获取所有tree对象
    std::vector<std::string> tree_objects = Commit::read_tree_object(repo.get_project_path());
    for (auto& file : tree_objects) {
        std::cout << file << std::endl;
    }
    // 将tree对象中包含的所有文件全部上传至服务器
}