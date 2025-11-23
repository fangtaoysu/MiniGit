#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "presentation/validator.h"
#include "presentation/model.h"
#include "application/cmd_executor.h"

namespace minigit::presentation {
class CommandEngine {
public:
    void RegisterCommand(const std::string& command_name,
                                std::unique_ptr<Validator> validator,
                                std::unique_ptr<minigit::application::CmdExecutor> executor);
    void Execute(const std::string& command_str);
                                
private:
    using CommandHandler = std::tuple<std::unique_ptr<Validator>, std::unique_ptr<minigit::application::CmdExecutor>>;
    std::unordered_map<std::string, CommandHandler> command_registry_;
};
}  // namespace minigit::presentation
