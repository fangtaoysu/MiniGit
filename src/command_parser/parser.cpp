#include "../../include/command_parser/parser.h"
#include "../../include/utils.h"
#include <iostream>
#include <stdexcept>
#include <unordered_set>


void StateMachineParser::handle_token(const std::string& token) {
    switch (current_state_) {
        case State::Name:
            current_cmd_.name = token;
            current_cmd_.is_key_value = is_key_value_command(token);
            current_state_ = State::Option; // 下一个状态先设为option
            break;

        case State::Option:
            if (current_cmd_.is_key_value) {
                if (token[0] == '-') { // 是option
                    current_cmd_.options.push_back(token);
                    current_state_ = State::ArgKey;
                } else { // 是key命令 && 不是option =》 是key
                    current_arg_key_ = token;
                    current_state_ = State::ArgValue;
                }
            } else { // 不含key-value参数的命令
                if (token[0] == '-') { // 是option
                    current_cmd_.options.push_back(token);
                } else { // 是plain
                    current_cmd_.plain_args.push_back(token);
                }
                current_state_ = State::PlainArg;
            }
            break;
        
        case State::ArgKey:
            current_arg_key_ = token;
            current_state_ = State::ArgValue;
            break;

        case State::ArgValue:
            current_cmd_.key_value_args[current_arg_key_] = token;
            current_state_ = State::Option;
            break;

        case State::PlainArg:
            current_cmd_.plain_args.push_back(token);
            current_state_ = State::Option;
    }
}

bool StateMachineParser::is_git_command(const std::string& cmd) {
    // 有新命令再加
    static const std::unordered_set<std::string> git_commands = {
        "init", "add", "commit", "config", "status",
        "push"
    };
    return git_commands.count(cmd) > 0;
}

void StateMachineParser::reset() {
    current_state_ = State::Name;
    invalid_handle = false;
    // 清空结构体
    current_cmd_.name.clear();
    current_cmd_.options.clear();
    current_cmd_.key_value_args.clear();
    current_cmd_.plain_args.clear();
    current_arg_key_.clear();
    current_cmd_.is_key_value = false;
}

ParsedCommand StateMachineParser::parse(const std::string& input) {
    reset();
    std::vector<std::string> tokens = Utils::tokenize(input);
    // std::cout << "input parse result:\n";
    // for (auto & token : tokens) {
    //     std::cout << token << "\t";
    // }
    // 跳过 "mgit" 前缀
    size_t start_idx = 0;
    if (!tokens.empty() && tokens[0] == "mgit") {
        start_idx = 1;
    }
    
    for (size_t i = start_idx; i < tokens.size(); ++i) {
        handle_token(tokens[i]);
    }
    
    if (invalid_handle || !is_git_command(current_cmd_.name)) {
        reset();
        throw std::runtime_error("Invalid git command: " + current_cmd_.name);
    }
    return current_cmd_;
}

bool StateMachineParser::is_key_value_command(const std::string& cmd) {
    static std::unordered_set<std::string> kv_cmds= {
        "config"
    };
    return kv_cmds.count(cmd) > 0;
}