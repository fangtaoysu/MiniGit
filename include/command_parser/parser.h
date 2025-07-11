#ifndef __PARSER_H__
#define __PARSER_H__

#include "types.h"


class StateMachineParser {
    public:
        StateMachineParser()=default;
        ParsedCommand parse(const std::string& input);

    private:
        enum class State {
            Start, Command, Arg, OptionKey, OptionValue
        };
        void reset();
        void handle_token(const std::string& token);
        State state_ = State::Start;
        ParsedCommand current_cmd_;
        std::string current_option_;
        bool is_git_command(const std::string& cmd);
};

#endif   