#include "../../../include/command_parser/commands/cmd_add.h"
#include "../../../include/index.h"
#include <iostream>


/** 实现 git add */
void CmdAdd::execute(const ParsedCommand& cmd, Repository& repo) {
    checkout_cmd(cmd);
    mgit_add(cmd.plain_args, repo.get_project_path());
}

void CmdAdd::mgit_add(const std::vector<std::string>& paths, const std::string& project_path) {
    Index index_obejct(project_path);
    if (paths.size() == 1 && paths[0] == ".") {
        index_obejct.add(nullptr);
    } else { // 遍历参数，add files的绝对地址
        std::vector<std::filesystem::path> files;
        for (auto & file : paths) {
            // 此处需要判断，如果参数是目录，那么需要递归拿到这个目录下的所有文件
            const std::string file_path(project_path + "/" + file);
            if (fs::is_directory(file_path)) {
                for (const auto & entry : fs::recursive_directory_iterator(file_path)) {
                    if (!fs::is_directory(entry)) {
                        files.push_back(entry);
                    }
                }
            } else {
                files.push_back(file_path); // 修复issue6
            }
        }
        for (const auto & file : files) {
            std::cout << file << std::endl;
        }
        index_obejct.add(&files);
    }
}

void CmdAdd::checkout_cmd(const ParsedCommand& cmd) {
    // 先不容许add 传 options
    if (!cmd.options.empty() || !cmd.key_value_args.empty()) {
        throw std::runtime_error("mgit add command error, unvalid args");
    }
}
