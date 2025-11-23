#pragma once

#include <string>
#include <vector>

#include "presentation/model.h"

namespace minigit::presentation {
/**
 * 验证器：用于验证命令是否符合语法规则
 */
class Validator {
public:
    virtual bool Validate(const CommandContext& cmd_context) = 0;
};
}  // namespace minigit::presentation