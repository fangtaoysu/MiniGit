#ifndef __CMD_ADD_H__
#define __CMD_ADD_H__
#include "../command.h"

class CmdAdd : public GitCommand {
    public:
        CmdAdd()=default;
        void execute(const ParsedCommand& cmd, Repository& repo) override;

    private:
};


#endif