#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
#include <vector>
#include <unordered_map>


struct ParsedCommand {
    std::string name; // add / commit
    std::vector<std::string> args;
    std::unordered_map<std::string, std::string> options;
};

using CommandArgs = ParsedCommand;

#endif