#pragma once

#include <string>

namespace presentation {

enum class TokenType {
    kIdentifier,      // e.g., git, commit, add
    kShortFlag,       // e.g., -m
    kLongFlag,        // e.g., --message
    kValue,           // e.g., a file path, or a value for a flag
    kQuotedValue,     // e.g., "a message with spaces"
    kEquals,          // The '=' in --flag=value
    kEndOfFile,       // Marks the end of the input string
    kUnknown          // An unrecognized token
};

struct Token {
    TokenType type;
    std::string lexeme; // The actual text of the token
    size_t position;    // Starting position in the original string for error reporting
};

} // namespace presentation