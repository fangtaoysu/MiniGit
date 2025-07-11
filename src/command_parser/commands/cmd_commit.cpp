#include "../../../include/command_parser/commands/cmd_commit.h"
#include "../../../include/commit.h"
#include <iostream>



void CmdCommit::execute(const ParsedCommand& cmd, Repository& repo) {
    if (!is_needed_execute(cmd)) {
        return;
    }
    Commit commit_obejct(repo.get_project_path());
    commit_obejct.run(cmd.options.at("-m"), "");
}

bool CmdCommit::is_needed_execute(const ParsedCommand& cmd) {
    // 检查参数问题
    auto it = cmd.options.find("-m");
    if (it == cmd.options.end() || it->second.empty()) { // 不容许msg为空
        std::cerr << "Aborting commit due to empty commit message.\n";
        return false;
    }
    // 检查暂存区中是否有内容
    
    return true;
}