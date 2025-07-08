#include "../../include/command_parser/parser.h"
#include "../../include/utils.h"
#include <exception>
#include <iostream>
#include <unordered_set>


void StateMachineParser::handle_token(const std::string& token) {
    switch (state_) {
        case State::Start:
            current_cmd_.name = token;
            state_ = State::Command;
            break;

        case State::Command:
        case State::Arg:
            if (token[0] == '-') {
                current_option_ = token;
                state_ = State::OptionKey;
            } else {
                current_cmd_.args.push_back(token);
                state_ = State::Arg;
            }
            break;
        
        case State::OptionKey:
            if (token[0] == '-') {
                current_cmd_.options[current_option_] = "";
                current_option_ = token;
            } else {
                current_cmd_.options[current_option_] = token;
                state_ = State::OptionValue;
            }
            break;

        case State::OptionValue:
            if (token[0] == '-') {
                current_option_ = token;
                state_ = State::Arg;
            }
            break;
    }
}

bool StateMachineParser::is_git_command(const std::string& cmd) {
    // 有新命令再加
    static const std::unordered_set<std::string> git_commands = {
        "init", "add", "commit"
    };
    return git_commands.count(cmd) > 0;
}

void StateMachineParser::reset() {
    state_ = State::Start;
    // 清空结构体
    current_cmd_.name.clear();
    current_cmd_.args.clear();
    current_cmd_.options.clear();
    current_option_.clear();
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
    
    if (!is_git_command(current_cmd_.name)) {
        throw std::runtime_error("Invalid git command: " + current_cmd_.name);
    }
    return current_cmd_;
}