#ifndef __CMD_STATUS_H__
#define __CMD_STATUS_H__
#include "../parser.h"
#include "../command.h"
#include "../../repository.h"
#include "../../index.h"


class CmdStatus : public GitCommand {
    public:
        CmdStatus()=default;
        void execute(const ParsedCommand& cmd, Repository& repo) override;

};

#endif