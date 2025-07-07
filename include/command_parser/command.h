#ifndef __COMMAND_H__
#define __COMMAND_H__
#include "types.h"
#include <vector>
#include "../repository.h"


class GitCommand {
    public:
        virtual ~GitCommand()=default;
        virtual void execute(const ParsedCommand& cmd, Repository& repo)=0;

    protected:
        void vaildate_options(const ParsedCommand& cmd, const std::pmr::vector<std::string>& allow_options);
};

#endif