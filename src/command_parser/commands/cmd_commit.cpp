#include "../../../include/command_parser/commands/cmd_commit.h"
#include "../../../include/commit.h"
#include <iostream>



void CmdCommit::execute(const ParsedCommand& cmd, Repository& repo) {
    checkout_cmd(cmd);
    Commit commit_obejct(repo.get_project_path());
    commit_obejct.run(cmd.plain_args[0]);
}


void CmdCommit::checkout_cmd(const ParsedCommand& cmd) {
    if (cmd.plain_args.empty()) {
        throw std::runtime_error("commit command too less args");
    } else if (cmd.plain_args.size() > 1) {
        throw std::runtime_error("commit command too more args");
    }
    if (cmd.options.size() != 1 || cmd.options[0] != "-m") {
        throw std::runtime_error("commit arg error, not found '-m'");
    } 
    if (!cmd.key_value_args.empty()) {
        throw std::runtime_error("commit command error, unvalid args");
    }
}
