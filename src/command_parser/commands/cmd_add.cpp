#include "../../../include/command_parser/commands/cmd_add.h"
#include "../../../include/index.h"
#include <iostream>


/** 实现 git add */
void CmdAdd::execute(const ParsedCommand& cmd) {
    std::string project_path = Utils::get_project_path();
    checkout_cmd(cmd, project_path);
    mgit_add(cmd.plain_args, project_path);
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

void CmdAdd::checkout_cmd(const ParsedCommand& cmd, const std::string& project_path) {
    // 先不容许add 传 options
    if (!cmd.options.empty() || !cmd.key_value_args.empty()) {
        throw std::runtime_error("mgit add command error, unvalid args");
    }

    // 修复issue7
    // 其绝对路径只能是.mgit路径上一级的子目录
    for (auto & file : cmd.plain_args) {
        fs::path file_path = fs::weakly_canonical(fs::path(project_path) / file);
        std::cout << file_path << std::endl;
        if (!Utils::is_subpath(project_path, file_path)) {
            throw std::runtime_error("fatal: not a git repository (or any of the parent directories): .mgit");
        } 
    }
}
