#include "application/init/init_executor.h"

#include <fstream>
#include <iostream>

#include "infrastructure/logging/logger.h"
#include "shared/path_utils.h"

namespace minigit::application::init {

bool InitExecutor::Execute(const CommandContext& cmd_context) {
    // 1. 创建对应文件
    InitRepo();
    CreateRepoFiles();
    LOG_INFO("Repository files created successfully");

    // 2. 创建对应数据库表
    return true;
}

void InitExecutor::InitRepo() {
    std::filesystem::path repo =
        minigit::shared::GetProjectRoot() / "build" / ".mgit";
    CreateRepoDir(repo);

    // 创建目录
    std::filesystem::path branches = repo / "branches";
    std::filesystem::path hooks = repo / "hooks";
    std::filesystem::path info = repo / "info";
    std::filesystem::path objects = repo / "objects";
    std::filesystem::path refs = repo / "refs";

    CreateRepoDir(branches);
    CreateRepoDir(hooks);
    CreateRepoDir(info);
    CreateRepoDir(objects);
    CreateRepoDir(refs);
}

void InitExecutor::CreateRepoDir(const std::filesystem::path& dir_path) {
    if (!std::filesystem::exists(dir_path)) {
        std::filesystem::create_directories(dir_path);
        LOG_INFO("Created repository directory: " << dir_path.string());
    }
}

void InitExecutor::CreateRepoFiles() {
    std::filesystem::path repo =
        minigit::shared::GetProjectRoot() / "build" / ".mgit";
    std::filesystem::path HEAD_path = repo / "HEAD";
    std::filesystem::path config = repo / "config";
    std::filesystem::path description = repo / "description";

    std::ofstream HEAD_file(HEAD_path);
    HEAD_file << "ref: refs/heads/main\n";
    HEAD_file.close();

    std::ofstream config_file(config);
    config_file << "[core]\n";
    config_file << "\trepositoryformatversion = 0\n";
    config_file << "\tfilemode = true\n";
    config_file << "\tbare = false\n";
    config_file << "\tlogallrefupdates = true\n";
    config_file.close();

    std::ofstream description_file(description);
    description_file << "Unnamed repository; edit this file 'description' to "
                        "name the repository.\n";
    description_file.close();
}

}  // namespace minigit::application::init