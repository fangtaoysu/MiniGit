#include "../../../include/command_parser/commands/cmd_config.h"
#include <iostream>



void CmdConfig::execute(const ParsedCommand& cmd) {
    checkout_cmd(cmd);
    Repository repo;
    repo.config(cmd.key_value_args);
}

void CmdConfig::checkout_cmd(const ParsedCommand& cmd) {
    if (!cmd.plain_args.empty()) {
        throw std::runtime_error("config command too more args");
    }
    if (cmd.options.size() != 1 || cmd.options[0] != "--local") {
        throw std::runtime_error("config arg error, not found '--local'");
    } 
    if (cmd.key_value_args.empty()) {
        throw std::runtime_error("config command error, not input config information.");
    }
}

