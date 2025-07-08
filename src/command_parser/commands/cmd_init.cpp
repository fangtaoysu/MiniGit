#include "../../../include/command_parser/commands/cmd_init.h"



/** 实现 git init */
void CmdInit::execute(const ParsedCommand& cmd, Repository& repo) {
    // 先不校验
    // std::cout << "init 进来了\n";
    repo.init_repository();
}


