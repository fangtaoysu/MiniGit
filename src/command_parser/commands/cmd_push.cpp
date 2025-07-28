#include "../../../include/command_parser/commands/cmd_push.h"
#include <iostream>



void CmdPush::execute(const ParsedCommand& cmd, Repository& repo) {
    // 获取commit对象，tree对象及blob对象
    const std::string commit_hash = Commit::read_commit_hash(repo.get_project_path());
    std::vector<std::string> tree_objects = Commit::read_tree_object(repo.get_project_path());
    std::unordered_map<std::string, std::string> tree_and_hash_map = get_tree_and_hash_map(tree_objects, repo.get_project_path());
    // 将tree对象中包含的所有文件全部上传至服务器

}


/** 将路径与index中对应的哈希构造为map */
std::unordered_map<std::string, std::string> CmdPush::get_tree_and_hash_map(const std::vector<std::string>& tree_objects, const std::string& project_path) {
    json index = Index::get_index(project_path);
    std::unordered_map<std::string, std::string> res;
    for (auto& tree : tree_objects) {
        if (index.contains(tree) && index[tree].contains("hash")) {
            res[tree] = index[tree]["hash"];
        } else {
            std::cerr << "缺失的哈希：" << tree << std::endl;
        }
    }
    return res;
}
