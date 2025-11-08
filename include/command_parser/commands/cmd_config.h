#ifndef __CMD_CONFIG_H__
#define __CMD_CONFIG_H__
#include "../command.h"
#include"../../repository.h"


class CmdConfig : public GitCommand {
    public:
        CmdConfig()=default;
        void execute(const ParsedCommand& cmd) override;
    private:
        void checkout_cmd(const ParsedCommand& cmd);
};


#endif