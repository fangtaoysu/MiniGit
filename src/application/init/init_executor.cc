#include "application/init/init_executor.h"

#include <filesystem>
#include <iostream>

#include "infrastructure/logging/logger.h"

namespace minigit::application::init {

InitExecutor::InitExecutor(std::shared_ptr<InitService> init_service)
    : init_service_(std::move(init_service)) {
}

bool InitExecutor::Execute(const CommandContext& cmd_context) {
    LOG_INFO("Executing git init command");
    
    // 获取仓库路径（从args或file_paths，默认为当前目录）
    std::string repo_path = ".";
    if (!cmd_context.args.empty()) {
        repo_path = cmd_context.args[0];
    } else if (!cmd_context.file_paths.empty()) {
        repo_path = cmd_context.file_paths[0];
    }
    
    // 获取默认分支名（从opts中解析，如 --initial-branch=main）
    std::string default_branch = "main";
    for (const auto& opt : cmd_context.opts) {
        if (opt.find("--initial-branch=") == 0) {
            default_branch = opt.substr(17); // 跳过 "--initial-branch="
            break;
        } else if (opt == "-b" && !cmd_context.args.empty()) {
            // 处理 -b branch_name 格式
            default_branch = cmd_context.args[0];
            break;
        }
    }
    
    // 调用InitService执行初始化
    bool success = init_service_->InitializeRepository(repo_path, default_branch);
    
    if (success) {
        LOG_INFO("Repository initialized successfully at: " << repo_path);
        std::cout << "Initialized empty Git repository in " 
                  << std::filesystem::absolute(repo_path) << std::endl;
    } else {
        LOG_ERROR("Failed to initialize repository at: " << repo_path);
    }
    
    return success;
}

} // namespace minigit::application::init