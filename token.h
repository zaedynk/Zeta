#pragma once
#include <string>
#include <string_view>

enum class TokenType {
    PLUS,
    MINUS,
    STAR,
    SLASH,
    COMMA,
    SEMICOLON,
    EQUAL,
    LESS_THAN,
    GREATER_THAN,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,
    IDENTIFIER,
    NUMBER,
    STRING,
    LET,
    FN,
    RETURN,
    IF,
    ELSE,
    WHILE,
    FOR,
    PRINT,
    TRUE,
    FALSE,
    NOT,
    NIL,
    EOF_TOKEN

};

std::string token_type_to_string(TokenType token_type);

struct Token {
    TokenType token_type;
    std::string_view lexeme;
    int line;
};
