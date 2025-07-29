#ifndef __CMD_INIT_H__
#define __CMD_INIT_H__

#include "../command.h"
#include "../../utils.h"
#include "../../repository.h"


class CmdInit : public GitCommand {
    public:
        CmdInit()=default;
        void execute(const ParsedCommand& cmd) override;
    private:
        void checkout_cmd(const ParsedCommand& cmd);
};

#endif