#include "../../../include/command_parser/commands/cmd_add.h"
#include "../../../include/index.h"


/** 实现 git add */
void CmdAdd::execute(const ParsedCommand& cmd, Repository& repo) {
    const std::string project_path = repo.get_project_path();
    Index index_obejct(project_path);
    if (cmd.args.size() == 1 && cmd.args[0] == ".") {
        index_obejct.add(nullptr);
    } else { // 遍历参数，add files的绝对地址
        std::vector<std::filesystem::path> files;
        for (auto & file : cmd.args) {
            files.push_back(project_path + "/" + file);
        }
        index_obejct.add(&files);
    }
}