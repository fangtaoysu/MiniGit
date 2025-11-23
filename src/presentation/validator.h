#pragma once

#include <string>
#include <vector>

#include "shared/model.h"

namespace minigit::presentation {
/**
 * 验证器：用于验证命令是否符合语法规则
 */
class Validator {
public:
    virtual bool Validate(const LexicalResult& cmd) = 0;
};
}  // namespace minigit::presentation