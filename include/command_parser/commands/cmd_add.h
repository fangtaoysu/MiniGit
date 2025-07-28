#ifndef __CMD_ADD_H__
#define __CMD_ADD_H__
#include "../command.h"

class CmdAdd : public GitCommand {
    public:
        CmdAdd()=default;
        void execute(const ParsedCommand& cmd, Repository& repo) override;

    private:
        void checkout_cmd(const ParsedCommand& cmd, const std::string& project_path);
        void mgit_add(const std::vector<std::string>& paths, const std::string& project_path);
};


#endif