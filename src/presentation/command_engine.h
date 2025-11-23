#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "presentation/validator.h"

class CommandEngine {
public:
    static void RegisterCommand(const std::string& command_name,
                                std::unique_ptr<Validator> validator);
    static void ValidateCommand(const LexicalResult& cmd);
    // void ExecuteCommand();

private:
    static std::unordered_map<std::string, std::unique_ptr<Validator>>
        validators_;
};
