#include "presentation/validator/init_validator.h"


namespace minigit::presentation::validator {
bool InitValidator::Validate(const CommandContext& cmd_context) {
    // init命令不接受任何参数 
    if (!cmd_context.opts.empty() || !cmd_context.args.empty() ||
        !cmd_context.file_paths.empty()) {
        return false;
    }
    return true;

}

} // namespace minigit::presentation::validator