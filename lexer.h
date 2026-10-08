#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "token.h"

class Lexer {
    std::vector<Token> tokens;
    std::size_t start_index = 0;
    std::size_t current_index = 0;
    int line = 1;
    std::string source_text = "";

public:

    explicit Lexer(std::string source_text);

    bool is_at_end() const;

    // Consume and return the current character
    char advance();

    // Look at the current character without consuming it
    char peek() const;

    char peek_further() const;

    std::vector<Token> lex();

    void scan_token();

    void add_token(TokenType type);
};
