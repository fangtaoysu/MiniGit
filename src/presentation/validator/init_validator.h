#pragma once

#include <string>

#include "presentation/validator.h"
#include "presentation/model.h"

using CommandContext = minigit::presentation::CommandContext;

namespace minigit::presentation::validator {
class InitValidator : public Validator {
public:
    bool Validate(const CommandContext& cmd_context) override;
};
}  // namespace minigit::presentation::validator