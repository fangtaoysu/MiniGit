#include "../../../include/command_parser/commands/cmd_push.h"
#include <iostream>
#include <sys/stat.h>



void CmdPush::execute(const ParsedCommand& cmd) {
    std::string project_path = Utils::get_project_path();
    Remote remote_object;
    
    // 构造commits需要的对象
    const std::string commit_hash = Commit::get_commit_hash();
    std::string tree_hash = Commit::get_tree_hash();
    ObjectDB object_db;
    json commit_object = object_db.read<json>(commit_hash);
    remote_object.push_commits(commit_hash, tree_hash, commit_object);

    // 构造files需要的对象
    std::vector<std::string> tree_objects = Commit::read_tree_object();
    std::unordered_map<std::string, json> file_tree_and_object_map = get_file_tree_and_object_map(tree_objects);
    remote_object.push_files(file_tree_and_object_map);

    // 构造commit_file_map需要的对象
    remote_object.push_commit_file_map(tree_objects, tree_hash);
}


/** 将路径与index中对应的哈希构造为map */
std::unordered_map<std::string, json> CmdPush::get_file_tree_and_object_map(const std::vector<std::string>& tree_objects) {
    const std::string project_path = Utils::get_project_path();
    json index = Index::get_index();
    std::unordered_map<std::string, json> res;
    for (auto& tree : tree_objects) {
        std::cout << tree << std:: endl;
        if (index.contains(tree)) {
            res[tree] = index[tree];
        } else {
            std::cerr << "缺失的文件信息：" << tree << std::endl;
        }
    }
    return res;
} 
