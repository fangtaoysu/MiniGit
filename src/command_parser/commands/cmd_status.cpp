#include "../../include/command_parser/commands/cmd_status.h"
#include <string>



void CmdStatus::execute(const ParsedCommand& cmd) {
    std::string project_path = Utils::get_project_path();
    Index index_object;
    index_object.status();
}