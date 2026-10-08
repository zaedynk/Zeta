#include <unordered_map>
#include <cctype>
#include <iostream>
#include "lexer.h"

static const std::unordered_map<std::string_view, TokenType> keywords = {
        {"let", TokenType::LET},
        {"fn", TokenType::FN},
        {"return", TokenType::RETURN},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"print", TokenType::PRINT},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"nil", TokenType::NIL}
};

Lexer::Lexer(std::string source_text) {
    this->source_text = std::move(source_text);
}

bool Lexer::is_at_end() const {
    return (current_index >= source_text.size());
}

// Consume and return the current character
char Lexer::advance() {
    char current_char = source_text[current_index];
    current_index += 1;
    return current_char;
}

// Look at the current character without consuming it
char Lexer::peek() const {
    if (is_at_end()) {
        return '\0';
    }

    char next_char = source_text[current_index];

    return next_char;
}

char Lexer::peek_further() const {
    // end check with the further peek
    if (current_index + 1 >= source_text.size()) {
        return '\0';
    }

    char next_char = source_text[current_index + 1];

    return next_char;
}

std::vector<Token> Lexer::lex() {

    while (!is_at_end()) {
        start_index = current_index;
        scan_token();
    }

    start_index = current_index;

    add_token(TokenType::EOF_TOKEN);

    return tokens;
}

void Lexer::scan_token() {
    char character = advance();

    switch (character) {
    case ('+'):
        add_token(TokenType::PLUS);
        break;
    case ('-'):
        add_token(TokenType::MINUS);
        break;
    case ('*'):
        add_token(TokenType::STAR);
        break;
    case ('/'):
        if (peek() == '/') {
            while (!is_at_end() && peek() != '\n') {
                advance();
            }
        }
        else {
            add_token(TokenType::SLASH);
        }
        break;
    case (','):
        add_token(TokenType::COMMA);
        break;
    case (';'):
        add_token(TokenType::SEMICOLON);
        break;
    case ('='):
        if (peek() == '=') {
            advance();
            add_token(TokenType::EQUAL_EQUAL);
        }
        else {
            add_token(TokenType::EQUAL);
        }
        break;
    case ('<'):
        if (peek() == '=') {
            advance();
            add_token(TokenType::LESS_EQUAL);
        }
        else {
            add_token(TokenType::LESS_THAN);
        }
        break;
    case ('>'):
        if (peek() == '=') {
            advance();
            add_token(TokenType::GREATER_EQUAL);
        }
        else {
            add_token(TokenType::GREATER_THAN);
        }
        break;
    case ('('):
        add_token(TokenType::LEFT_PAREN);
        break;
    case (')'):
        add_token(TokenType::RIGHT_PAREN);
        break;
    case ('{'):
        add_token(TokenType::LEFT_BRACE);
        break;
    case ('}'):
        add_token(TokenType::RIGHT_BRACE);
        break;
    case ('['):
        add_token(TokenType::LEFT_BRACKET);
        break;
    case (']'):
        add_token(TokenType::RIGHT_BRACKET);
        break;
    case ('!'):
        if (peek() == '=') {
            advance();
            add_token(TokenType::NOT_EQUAL);
        }
        else {
            add_token(TokenType::NOT);
        }
        break;
    case ('"'):
        while (!is_at_end() && peek() != '"') {
            if (peek() == '\n') {
                line += 1;
            }
            advance();
        }
        if (peek() != '"') {
            std::cerr << "Error on line " << line << ": unterminated string \n";
        }
        else {
            advance();
            add_token(TokenType::STRING);
        }
        break;
    case ('\n'):
        line += 1;
        break;
    case (' '):
    case ('\t'):
    case ('\r'):
        break;
    default:
        if (std::isdigit(character)) {
            while (!is_at_end() && std::isdigit(peek())) {
                advance();
            }
            if (peek() == '.' && std::isdigit(peek_further())) {
                advance();
                while (!is_at_end() && std::isdigit(peek())) {
                    advance();
                }
            }
            add_token(TokenType::NUMBER);
        }
        else if (std::isalpha(character) || character == '_') {
            while (std::isalnum(peek()) || peek() == '_') {
                advance();
            }
            std::string_view word = std::string_view(source_text.data() + start_index, current_index - start_index);
            auto match = keywords.find(word);
            if (match != keywords.end()) {
                add_token(match->second);
            }
            else {
                add_token(TokenType::IDENTIFIER);
            }
        }
        else {
            std::cerr << "Error on line " << line << ": unexpected " << "'" << character << "'" << "\n";
        }
        break;
    }
}

void Lexer::add_token(TokenType type) {
    Token token;
    token.token_type = type;
    token.lexeme = std::string_view(source_text.data() + start_index, current_index - start_index); // a view of exactly the token's characters, pointing into source, which the lexer owns.
    token.line = line;
    tokens.push_back(token);
}