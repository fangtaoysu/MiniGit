#include "../include/command_parser/parser.h"
#include "../include/utils.h"
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
        "add", "commit"
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

    for (const auto& token : tokens) {
        handle_token(token);
    }
    return current_cmd_;
}