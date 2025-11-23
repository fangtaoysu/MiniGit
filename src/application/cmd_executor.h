#pragma once
#include <string>
#include <vector>

#include "presentation/model.h"

using CommandContext = minigit::presentation::CommandContext;

namespace minigit::application {

class CmdExecutor {
public:
    virtual bool Execute(const CommandContext& cmd_context) = 0;
    virtual ~CmdExecutor() = default;
};

}  // namespace minigit::application