#pragma once

#include <filesystem>
#include <memory>

#include "application/cmd_executor.h"

namespace minigit::application::init {
class InitExecutor : public minigit::application::CmdExecutor {
public:
    bool Execute(const CommandContext& cmd_context) override;

private:
    void InitRepo();
    void CreateRepoDir(const std::filesystem::path& dir_path);
    void CreateRepoFiles();
};

}  // namespace minigit::application::init