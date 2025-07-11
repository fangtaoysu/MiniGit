#include "../../../include/command_parser/commands/cmd_commit.h"
#include "../../../include/commit.h"
#include "index.h"
#include <iostream>



void CmdCommit::execute(const ParsedCommand& cmd, Repository& repo) {
    const std::string project_path = repo.get_project_path();
    Index index_object(project_path);
    if (!is_needed_execute(cmd)) {
        return;
    }
    Commit commit_obejct(project_path);
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
    
    if ()
    return true;
}