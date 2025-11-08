/** 项目入口 */
#include "include/command_parser/command.h"
#include "include/command_parser/command_dispatcher.h"
#include "include/command_parser/commands/cmd_init.h"
#include "include/command_parser/commands/cmd_add.h"
#include "include/command_parser/commands/cmd_commit.h"
#include "include/command_parser/commands/cmd_config.h"
#include "include/command_parser/commands/cmd_status.h"
#include "include/command_parser/commands/cmd_push.h"
#include "include/repository.h"
#include "include/index.h"
#include "include/commit.h"
#include "include/command_parser/parser.h"
#include <iostream>
#include <memory>



std::string control_input() {
    std::string input;
    std::cout << ">>> ";
    if (!std::getline(std::cin,  input)) {
        return "EOF"; // 标记ctrl+d
    }
    return input; 
}

void run() {
    std::string cmd;
    std::unique_ptr<GitCommand> git_cmd;
    CommandDispatcher dispatcher;
    // 由状态机完成命令解析的部分
    StateMachineParser parser;
    //  creates a std::unique_ptr object
    dispatcher.register_command("init", std::make_unique<CmdInit>());
    dispatcher.register_command("add", std::make_unique<CmdAdd>());
    dispatcher.register_command("commit", std::make_unique<CmdCommit>());
    dispatcher.register_command("config", std::make_unique<CmdConfig>());
    dispatcher.register_command("status", std::make_unique<CmdStatus>());
    dispatcher.register_command("push", std::make_unique<CmdPush>());
    std::string input;
    input = control_input();
    
    while (input != "exit()" && input != "EOF") {
        if (input.find("exit") != std::string::npos) {
            std::cout << "Use exit() or Ctrl-D (i.e. EOF) to exit\n";
            input = control_input();
            continue;
        }
        try {
            ParsedCommand cmd = parser.parse(input);
            dispatcher.execute(cmd);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        input = control_input();
    }

}

void origin_cmd_test() {
    const std::string project_path("/data/fangtao/MiniGit/tmp");
    Repository repo;
    repo.init_repository();
    Index index_object;
    index_object.add(nullptr);
    Commit commit_object;
    commit_object.run("mgit init");
    commit_object.run("feat:new add\n -look at me \n- 是不是两行");
}

int main() {
    run();
    return 0;
}