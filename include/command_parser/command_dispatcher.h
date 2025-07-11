#ifndef __COMMAND_DISPATCHER_H__
#define __COMMAND_DISPATCHER_H__



#include "command.h"
#include <memory>
#include <mutex>
class CommandDispatcher {
    public:
        CommandDispatcher(Repository& repo);
        void register_command(const std::string& name, std::unique_ptr<GitCommand> cmd);
        void execute(const ParsedCommand& cmd);

    private:
        Repository& repo_;
        std::unordered_map<std::string, std::unique_ptr<GitCommand>> commands_;
};

#endif