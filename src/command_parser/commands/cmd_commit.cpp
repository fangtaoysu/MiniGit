#include "../../../include/command_parser/commands/cmd_commit.h"
#include "../../../include/commit.h"



void CmdCommit::execute(const ParsedCommand& cmd, Repository& repo) {
    Commit commit_obejct(repo.get_project_path());
    commit_obejct.run(cmd.options.at("-m"), "");
}