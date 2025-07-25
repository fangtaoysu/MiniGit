#include "../../include/command_parser/commands/cmd_status.h"



void CmdStatus::execute(const ParsedCommand& cmd, Repository& repo) {
    Index index_object(repo.get_project_path());
    index_object.status();
}