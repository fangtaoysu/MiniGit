#pragma once

#include <string>

#include "presentation/model.h"
#include "presentation/validator.h"

using CommandContext = minigit::presentation::CommandContext;

namespace minigit::presentation::validator {
class InitValidator : public Validator {
public:
    bool Validate(const CommandContext& cmd_context) override;
};
}  // namespace minigit::presentation::validator