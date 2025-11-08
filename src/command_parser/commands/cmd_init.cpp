#include "../../../include/command_parser/commands/cmd_init.h"
#include <iostream>



/** 实现 git init */
void CmdInit::execute(const ParsedCommand& cmd) {
    checkout_cmd(cmd);
    Repository repo;
    repo.init_repository();
}

void CmdInit::checkout_cmd(const ParsedCommand& cmd) {
    if (!cmd.key_value_args.empty() || !cmd.plain_args.empty() || !cmd.options.empty()) {
        throw std::runtime_error("init command error, too much args");
    }
}
