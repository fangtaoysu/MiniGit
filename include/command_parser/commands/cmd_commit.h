#ifndef __CMD_COMMIT_H__
#define __CMD_COMMIT_H__

#include "../command.h"

class CmdCommit : public GitCommand {
    public:
        CmdCommit()=default;
        void execute(const ParsedCommand& cmd, Repository& repo) override;

    private:
        bool is_needed_execute(const ParsedCommand& cmd);
};

#endif