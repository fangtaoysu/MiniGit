#pragma once

#include <string>
#include <vector>

#include "shared/model.h"

/**
 * 词法分析器：将输入的命令字符串解析为结构化的命令表示形式
 */
class Lexer {
public:
    LexicalResult LexicalAnalyze(const std::string& command_str);

private:
    std::vector<std::string> Tokenize(const std::string& command_str);
    enum class CmdState { COMMAND, OPTION, ARGUMENT, FILE_PATH, END };
};