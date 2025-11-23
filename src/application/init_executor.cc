#include "application/init_executor.h"

#include "infrastructure/logging/logger.h"

namespace minigit::application {
bool InitExecutor::Execute(const CommandContext& cmd_context) {
    LOG_INFO("InitExecutor::Execute");
    return false;
}

}  // namespace minigit::application