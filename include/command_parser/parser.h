#ifndef __PARSER_H__
#define __PARSER_H__

#include "types.h"


class StateMachineParser {
    public:
        StateMachineParser()=default;
        ParsedCommand parse(const std::string& input);

    private:
        enum class State {
            Name, Option, ArgKey, ArgValue, PlainArg
        };
        void reset();
        void handle_token(const std::string& token);
        State current_state_ = State::Name;
        ParsedCommand current_cmd_;
        std::string current_arg_key_;
        bool invalid_handle = false;
        bool is_git_command(const std::string& cmd);
        bool is_key_value_command(const std::string& cmd);
};

#endif   