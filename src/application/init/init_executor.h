#pragma once

#include "application/cmd_executor.h"

namespace minigit::init::application {
class InitExecutor : public minigit::application::CmdExecutor {
public:
    bool Execute(const CommandContext& cmd_context) override;
};

}  // namespace minigit::init::application