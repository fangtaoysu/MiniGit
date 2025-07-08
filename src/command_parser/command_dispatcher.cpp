#include "../../include/command_parser/command_dispatcher.h"
#include <stdexcept>



CommandDispatcher::CommandDispatcher(Repository& repo)
    : repo_(repo) {

}

/**在main中注册command，让代码更具可读性 */
void CommandDispatcher::register_command(const std::string& name, std::unique_ptr<GitCommand> cmd) {
    // unique_ptr对象不允许复制，而且cmd会在函数生命周期结束后销毁，因此此处必须使用move
    commands_[name] = std::move(cmd);
}

/** 命令分发 */
void CommandDispatcher::execute(const ParsedCommand& cmd) {
    // 对比分发器中的key和用户输入解析后的name是否一致
    auto it = commands_.find(cmd.name);
    if (it != commands_.end()) {
        it->second->execute(cmd, repo_);
    } else {
        throw std::runtime_error("Unknown command: " + cmd.name);
    }
}