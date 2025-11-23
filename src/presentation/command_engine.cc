#include "presentation/command_engine.h"

#include "infrastructure/logging/logger.h"
#include "presentation/lexer.h"

namespace minigit::presentation {


void CommandEngine::RegisterCommand(const std::string& command_name,
                                    std::unique_ptr<Validator> validator,
                                    std::unique_ptr<minigit::application::CmdExecutor> executor) {
    command_registry_[command_name] = {std::move(validator), std::move(executor)};
}

void CommandEngine::Execute(const std::string& command_str) {
    LOG_INFO("start execute command: " << command_str);
    Lexer lexer;
    const CommandContext splited_cmd = lexer.LexicalAnalyze(command_str);
    LOG_INFO("splited cmd: " << splited_cmd.cmd);
    
    // NOTE: 如果用move，会移走注册表中的内容，下次 调用时就会报错
    // auto [validator, executor] = std::move(command_registry_[splited_cmd.cmd]);

    auto it = command_registry_.find(splited_cmd.cmd);
    if (it == command_registry_.end()) {
        LOG_ERROR("No command registered for command: " << splited_cmd.cmd);
        return;
    }

    auto& [validator, executor] = it->second;
    if (!validator) {
        LOG_ERROR("No validator registered for command: " << splited_cmd.cmd);
        return;
    }

    if (!executor) {
        LOG_ERROR("No executor registered for command: " << splited_cmd.cmd);
        return;
    }

    if (!validator->Validate(splited_cmd)) {
        LOG_ERROR("Validation failed for command: " << splited_cmd.cmd);
        return;
    }
    LOG_INFO("Command validation succeeded for command: " << command_str);

    try {
        executor->Execute(splited_cmd); 
    } catch (const std::exception& e) {
        LOG_ERROR("Execution failed for command: "
                  << command_str << " with error: " << e.what());
    }
}

}  // namespace minigit::presentation