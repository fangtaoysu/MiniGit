#include "presentation/command_engine.h"

#include "infrastructure/logging/logger.h"

std::unordered_map<std::string, std::unique_ptr<Validator>>
    CommandEngine::validators_;

void CommandEngine::RegisterCommand(const std::string& command_name,
                                    std::unique_ptr<Validator> validator) {
    validators_[command_name] = std::move(validator);
}

void CommandEngine::ValidateCommand(const LexicalResult& cmd) {
    // 命令未注册则写入日志
    if (!validators_.contains(cmd.command)) {
        LOG_WARN("No validator registered for command: " << cmd.command);
    }

    auto validator = std::move(validators_[cmd.command]);
    try {
        validators_->Validate(cmd);
    } catch (const std::exception& e) {
        LOG_ERROR("Validation failed for command: "
                  << cmd.command << " with error: " << e.what());
    }
}