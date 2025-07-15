#pragma once
#include <string>

namespace parallaxdb {

enum class TokenType {
    SELECT,
    FROM,
    WHERE,
    AND,
    OR,
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER,
    GREATER_THAN,
    LESS_THAN,
    EQUALS,
    GREATER_EQUAL,
    LESS_EQUAL,
    NOT_EQUALS,
    STAR,
    COMMA,
    SEMICOLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    END_OF_INPUT,
    ERROR
};

struct Token {
    TokenType type;
    std::string value;
    size_t position;
    Token(TokenType t, const std::string& v, size_t pos)
        : type(t), value(v), position(pos) {}
};

} // namespace parallaxdb 