#ifndef __CMD_PUSH_H__
#define __CMD_PUSH_H__
#include "../command.h"
#include "../parser.h"


class CmdPush : public GitCommand {
    public:
        void execute(const ParsedCommand& cmd, Repository& repo) override;
};

#endif