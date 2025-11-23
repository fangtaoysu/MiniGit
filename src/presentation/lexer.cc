#include "presentation/lexer.h"

#include <sstream>

#include "infrastructure/logging/logger.h"

LexicalResult Lexer::LexicalAnalyze(const std::string& command_str) {
    LOG_INFO("Lexical analyzing command: " << command_str);
    LexicalResult result;
    std::vector<std::string> tokens = Tokenize(command_str);

    if (tokens.empty()) {
        LOG_WARN("No tokens found in command: " << command_str);
        return result;
    }
    // 检查git并跳过git前缀
    auto it = tokens.begin();
    if (*it != "git") {
        LOG_WARN("Command should start with 'git', cmd: " << command_str);
        return result;
    }
    ++it;

    CmdState current_state = CmdState::COMMAND;
    CmdState next_state = CmdState::OPTION;
    while (it != tokens.end()) {
        const std::string& token = *it;

        if (current_state == CmdState::END) {
            LOG_WARN(
                "Unexpected token after arguments and file paths: " << token);
            break;
        }

        // 如果当前下一个状态不是-开头，那继续按默认的文件路径
        if (token.front() == '-') {
            current_state = CmdState::OPTION;
        }
        switch (current_state) {
            case CmdState::COMMAND:
                result.command = token;
                LOG_INFO("Parsed command: " << token);
                // 设置下一个状态
                next_state = CmdState::FILE_PATH;
                break;

            case CmdState::OPTION:
                result.option.push_back(token);
                LOG_INFO("Parsed option: " << token);
                next_state = CmdState::ARGUMENT;
                break;

            case CmdState::ARGUMENT:
                result.argument.push_back(token);
                LOG_INFO("Parsed argument: " << token);
                next_state = CmdState::END;
                break;

            case CmdState::FILE_PATH:
                result.file_path.push_back(token);
                LOG_INFO("Parsed file path: " << token);
                next_state = CmdState::FILE_PATH;
                break;

            default:
                break;
        }
        current_state = next_state;
        ++it;
    }

    return result;
}

std::vector<std::string> Lexer::Tokenize(const std::string& command_str) {
    std::vector<std::string> tokens;
    std::istringstream stream(command_str);
    std::string token;

    while (stream >> token) {
        // 没有引号就按空格分隔
        if (token.find('"') == std::string::npos) {
            tokens.push_back(token);
        } else {
            std::string quoted_token =
                token +
                stream.str().substr(static_cast<size_t>(stream.tellg()));
            // 移除引号
            if (quoted_token.front() == '"') {
                quoted_token.erase(0, 1);
            }
            if (quoted_token.back() == '"') {
                quoted_token.pop_back();
            }
            tokens.push_back(quoted_token);
            break;
        }
    }

    LOG_INFO("Tokenized command " << command_str << "\tTokens:");
    std::string tokenized_str;
    for (const auto& t : tokens) {
        tokenized_str += "[" + t + "] ";
    }
    LOG_INFO("\t" << tokenized_str);
    return tokens;
}