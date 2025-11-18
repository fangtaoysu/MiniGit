#include "presentation/scanner.h"

#include <cctype>

namespace presentation {

Scanner::Scanner(const std::string& source) : source_(source) {}

std::vector<Token> Scanner::ScanTokens() {
    while (!IsAtEnd()) {
        start_ = current_;
        ScanToken();
    }
    tokens_.push_back({TokenType::kEndOfFile, "", source_.length()});
    return tokens_;
}

void Scanner::ScanToken() {
    char c = Advance();
    switch (c) {
        // Ignore whitespace
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            break;

        case '=':
            tokens_.push_back({TokenType::kEquals, "=", start_});
            break;

        case '"':
            HandleQuotedValue();
            break;

        case '-':
            HandleFlag();
            break;

        default: {
            while (!isspace(Peek()) && Peek() != '=' && !IsAtEnd()) {
                Advance();
            }
            std::string text = source_.substr(start_, current_ - start_);

            bool is_identifier = !text.empty();
            if (is_identifier) {
                for (char ch : text) {
                    if (!IsAlphaNumeric(ch)) {
                        is_identifier = false;
                        break;
                    }
                }
            }

            TokenType type = is_identifier ? TokenType::kIdentifier : TokenType::kValue;
            tokens_.push_back({type, text, start_});
            break;
        }
    }
}

void Scanner::HandleFlag() {
    if (Peek() == '-') { // Long flag, e.g., --message
        Advance(); // Consume the second '-'
        while (IsAlphaNumeric(Peek())) {
            Advance();
        }
        std::string text = source_.substr(start_, current_ - start_);
        tokens_.push_back({TokenType::kLongFlag, text, start_});
    } else { // Short flag, e.g., -m
        while (IsAlphaNumeric(Peek())) {
            Advance();
        }
        std::string text = source_.substr(start_, current_ - start_);
        tokens_.push_back({TokenType::kShortFlag, text, start_});
    }
}

void Scanner::HandleQuotedValue() {
  // The opening quote at `start_` is already consumed by Advance() in ScanToken().
  // `current_` is now at the first character of the value.
  const auto value_start = current_;
  while (Peek() != '"' && !IsAtEnd()) {
    // Handle escaped quotes
    if (Peek() == '\\' && PeekNext() == '"') {
      Advance();  // Consume '\'
    }
    Advance();
  }

  if (IsAtEnd()) {
    // Unterminated string.
    std::string text = source_.substr(value_start, current_ - value_start);
    tokens_.push_back({TokenType::kUnknown, text, start_});
    return;
  }

  // The closing quote.
  std::string text = source_.substr(value_start, current_ - value_start);
  tokens_.push_back({TokenType::kQuotedValue, text, start_});
  Advance();  // Consume the closing quote.
}


char Scanner::Advance() {
    if (IsAtEnd()) return '\0';
    return source_[current_++];
}

char Scanner::Peek() const {
    if (IsAtEnd()) return '\0';
    return source_[current_];
}

char Scanner::PeekNext() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

bool Scanner::IsAtEnd() const {
    return current_ >= source_.length();
}

bool Scanner::IsAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::IsDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Scanner::IsAlphaNumeric(char c) const {
    return IsAlpha(c) || IsDigit(c);
}

} // namespace presentation