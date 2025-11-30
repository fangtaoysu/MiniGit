#pragma once


#include <memory>

#include "application/cmd_executor.h"
#include "application/init/init_service.h"

namespace minigit::application::init {
class InitExecutor : public minigit::application::CmdExecutor {
public:
    explicit InitExecutor(std::shared_ptr<InitService> init_service);
    bool Execute(const CommandContext& cmd_context) override;

private:
    std::shared_ptr<InitService> init_service_;

};

}  // namespace minigit::application::init