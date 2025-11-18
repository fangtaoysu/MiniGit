#pragma once

#include <string>
#include <vector>

#include "presentation/token.h"

namespace presentation {

// The Scanner (or Lexer) is responsible for turning a raw command string
// into a sequence of tokens.
class Scanner {
public:
    explicit Scanner(const std::string& source);

    // Scans the entire source string and returns a vector of tokens.
    std::vector<Token> ScanTokens();

private:
    // Helper methods for scanning different token types.
    void ScanToken();
    void HandleIdentifier();
    void HandleFlag();
    void HandleQuotedValue();
    void HandleValue();

    // Helper methods for character inspection.
    char Advance();
    char Peek() const;
    char PeekNext() const;
    bool IsAtEnd() const;
    bool IsAlpha(char c) const;
    bool IsDigit(char c) const;
    bool IsAlphaNumeric(char c) const;

    const std::string source_;
    std::vector<Token> tokens_;
    size_t start_ = 0;
    size_t current_ = 0;
};

} // namespace presentation