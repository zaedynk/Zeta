#include "token.h"

std::string token_type_to_string(TokenType token_type) {
    switch (token_type) {
    case TokenType::PLUS:
        return "PLUS";
    case TokenType::MINUS:
        return "MINUS";
    case TokenType::STAR:
        return "STAR";
    case TokenType::SLASH:
        return "SLASH";
    case TokenType::COMMA:
        return "COMMA";
    case TokenType::SEMICOLON:
        return "SEMICOLON";
    case TokenType::EQUAL:
        return "EQUAL";
    case TokenType::LESS_THAN:
        return "LESS_THAN";
    case TokenType::GREATER_THAN:
        return "GREATER_THAN";
    case TokenType::LEFT_PAREN:
        return "LEFT_PAREN";
    case TokenType::RIGHT_PAREN:
        return "RIGHT_PAREN";
    case TokenType::LEFT_BRACE:
        return "LEFT_BRACE";
    case TokenType::RIGHT_BRACE:
        return "RIGHT_BRACE";
    case TokenType::LEFT_BRACKET:
        return "LEFT_BRACKET";
    case TokenType::RIGHT_BRACKET:
        return "RIGHT_BRACKET";
    case TokenType::EQUAL_EQUAL:
        return "EQUAL_EQUAL";
    case TokenType::NOT_EQUAL:
        return "NOT_EQUAL";
    case TokenType::LESS_EQUAL:
        return "LESS_EQUAL";
    case TokenType::GREATER_EQUAL:
        return "GREATER_EQUAL";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::STRING:
        return "STRING";
    case TokenType::LET:
        return "LET";
    case TokenType::FN:
        return "FN";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::FOR:
        return "FOR";
    case TokenType::PRINT:
        return "PRINT";
    case TokenType::TRUE:
        return "TRUE";
    case TokenType::FALSE:
        return "FALSE";
    case TokenType::NOT:
        return "NOT";
    case TokenType::NIL:
        return "NIL";
    case TokenType::EOF_TOKEN:
        return "EOF_TOKEN";
    default:
        return "UNKNOWN TOKEN";
    }
}