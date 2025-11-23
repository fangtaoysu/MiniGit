#pragma once

#include "application/cmd_executor.h"

namespace minigit::application {
class InitExecutor : public CmdExecutor {
public:
    bool Execute(const CommandContext& cmd_context) override;
};

}  // namespace minigit::application