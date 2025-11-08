#ifndef __CMD_STATUS_H__
#define __CMD_STATUS_H__
#include "../parser.h"
#include "../command.h"
#include "../../index.h"
#include "../../utils.h"
#include "../types.h"




class CmdStatus : public GitCommand {
    public:
        CmdStatus()=default;
        void execute(const ParsedCommand& cmd) override;

};

#endif