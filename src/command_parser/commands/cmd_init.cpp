#include "../../../include/command_parser/commands/cmd_init.h"
#include <iostream>



/** 实现 git init */
void CmdInit::execute(const ParsedCommand& cmd, Repository& repo) {
    // 先不校验
    // std::cout << "init 进来了\n";
    // 先手动传入路径，后面再优化这部分逻辑
    // if (!cmd.args.empty()) {
    //     repo.init_repository(cmd.args[0]);
    // }
    repo.init_repository();
}


