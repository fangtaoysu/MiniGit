/** 集成测试
 * 1. init run
 * 2. add run
 * 3. commit run
 */
#include <filesystem>
#include <string>
#include <stdexcept>
#include <iostream>
#include "../include/index.h"
#include "../include/commit.h"


namespace fs = std::filesystem;
void init_repository(const std::string& project_path) {
    if (!project_path.empty() && !fs::exists(project_path)) {
        throw std::runtime_error("项目路径不存在：" + project_path);
    }
    const fs::path mgit_path =  fs::path(project_path) / ".mgit";
    fs::create_directories(mgit_path);
}

int main() {
    const std::string project_path("/data/fangtao/MiniGit/tmp");
    init_repository(project_path);
    std::cout << "初始化完成" << std::endl;
    // 进行集成测试
    // add 测试
    Index index_obejct(project_path);
    index_obejct.add(nullptr);
    std::cout << "git add 完成，请检查index文件和objects文件夹" << std::endl;
    // commit 测试
    fs::remove(fs::path(project_path) / ".mgit/logs/HEAD");
    Commit commit_object(project_path);
    const std::string msg1("my mini git init, happy");
    const std::string msg2("feat: complish add and commit");
    commit_object.run(msg1);
    commit_object.run(msg2);
    std::cout << "git commit 完成，请检查logs/HEAD文件" << std::endl;
    return 0;
}