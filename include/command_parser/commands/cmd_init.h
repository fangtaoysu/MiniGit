#ifndef __CMD_INIT_H__
#define __CMD_INIT_H__

#include "../command.h"

class CmdInit : public GitCommand {
    public:
        CmdInit()=default;
        void execute(const ParsedCommand& cmd, Repository& repo) override;
    private:
        void checkout_cmd(const ParsedCommand& cmd);
};

#endif