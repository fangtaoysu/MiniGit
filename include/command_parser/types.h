#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
#include <vector>
#include <unordered_map>


struct ParsedCommand {
    std::string name; // add / commit
    std::vector<std::string> options; // -m / --local
    std::unordered_map<std::string, std::string> key_value_args; // config
    std::vector<std::string> plain_args; // 如add后面的路径
    bool is_key_value = false; // 默认没有键值对参数
};

using CommandArgs = ParsedCommand;

#endif