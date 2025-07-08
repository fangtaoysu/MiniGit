/** 项目入口 */
#include "./include/repository.h"
#include "./include/index.h"
#include "./include/commit.h"
#include <iostream>


int main() {
    const std::string project_path("/data/fangtao/MiniGit/tmp");
    Repository repo;
    repo.init_repository(project_path);
    Index index_object(project_path);
    index_object.add(nullptr);
    Commit commit_object(project_path);
    const std::string father_ref = commit_object.run("mgit init", "");
    const std::string next_ref = commit_object.run("feat:new add\n -look at me \n- 是不是两行", father_ref);
    std::cout << "next ref:" << next_ref << std::endl;
    return 0;
}