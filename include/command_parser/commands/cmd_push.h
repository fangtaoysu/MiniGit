#ifndef __CMD_PUSH_H__
#define __CMD_PUSH_H__
#include "../command.h"
#include "../parser.h"
#include "../../index.h"
#include "../../commit.h"
#include "../../remote.h"
#include "../../utils.h"


class CmdPush : public GitCommand {
    public:
        void execute(const ParsedCommand& cmd) override;
        ~CmdPush() override = default;  // 显式声明默认析构
    private:
        std::unordered_map<std::string, std::string> get_tree_and_hash_map(const std::vector<std::string>& tree_objects, const std::string& project_path);


};

#endif