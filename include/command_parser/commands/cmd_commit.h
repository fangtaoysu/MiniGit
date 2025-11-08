#ifndef __CMD_COMMIT_H__
#define __CMD_COMMIT_H__

#include "../command.h"
#include "../../utils.h"



class CmdCommit : public GitCommand {
    public:
        CmdCommit()=default;
        void execute(const ParsedCommand& cmd) override;

    private:
        void checkout_cmd(const ParsedCommand& cmd);
};

#endif