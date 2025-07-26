#ifndef __CMD_PUSH_H__
#define __CMD_PUSH_H__
#include "../command.h"
#include "../parser.h"
#include "../../commit.h"



class CmdPush : public GitCommand {
    public:
        void execute(const ParsedCommand& cmd, Repository& repo) override;
        ~CmdPush() override = default;  // 显式声明默认析构


};

#endif