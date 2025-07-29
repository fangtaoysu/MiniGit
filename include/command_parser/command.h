#ifndef __COMMAND_H__
#define __COMMAND_H__
#include "types.h"



class GitCommand {
    public:
        virtual ~GitCommand()=default;
        // 定义一个纯虚函数，规范派生类实现的接口
        virtual void execute(const ParsedCommand& cmd)=0;

    protected:
};

#endif