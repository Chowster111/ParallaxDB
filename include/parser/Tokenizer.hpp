#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include "Token.hpp"

namespace parallaxdb {

class Tokenizer {
public:
    Tokenizer(const std::string& input) : input(input), position(0) {}
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (position < input.length()) {
            skipWhitespace();
            
            if (position >= input.length()) break;
            
            char current = input[position];
            
            if (std::isalpha(current)) {
                tokens.push_back(readIdentifier());
            } else if (std::isdigit(current)) {
                tokens.push_back(readNumber());
            } else if (current == '\'') {
                tokens.push_back(readStringLiteral());
            } else if (current == '*') {
                tokens.push_back(Token(TokenType::STAR, "*", position++));
            } else if (current == ',') {
                tokens.push_back(Token(TokenType::COMMA, ",", position++));
            } else if (current == ';') {
                tokens.push_back(Token(TokenType::SEMICOLON, ";", position++));
            } else if (current == '(') {
                tokens.push_back(Token(TokenType::LEFT_PAREN, "(", position++));
            } else if (current == ')') {
                tokens.push_back(Token(TokenType::RIGHT_PAREN, ")", position++));
            } else if (current == '!') {
                if (position + 1 < input.length() && input[position + 1] == '=') {
                    tokens.push_back(Token(TokenType::NOT_EQUALS, "!=", position));
                    position += 2;
                } else {
                    tokens.push_back(Token(TokenType::ERROR, "!", position++));
                }
            } else if (current == '>') {
                if (position + 1 < input.length() && input[position + 1] == '=') {
                    tokens.push_back(Token(TokenType::GREATER_EQUAL, ">=", position));
                    position += 2;
                } else {
                    tokens.push_back(Token(TokenType::GREATER_THAN, ">", position++));
                }
            } else if (current == '<') {
                if (position + 1 < input.length() && input[position + 1] == '=') {
                    tokens.push_back(Token(TokenType::LESS_EQUAL, "<=", position));
                    position += 2;
                } else {
                    tokens.push_back(Token(TokenType::LESS_THAN, "<", position++));
                }
            } else if (current == '=') {
                tokens.push_back(Token(TokenType::EQUALS, "=", position++));
            } else {
                // Unknown character
                tokens.push_back(Token(TokenType::ERROR, std::string(1, current), position++));
            }
        }
        
        tokens.push_back(Token(TokenType::END_OF_INPUT, "", position));
        return tokens;
    }

private:
    std::string input;
    size_t position;
    
    void skipWhitespace() {
        while (position < input.length() && std::isspace(input[position])) {
            position++;
        }
    }
    
    Token readIdentifier() {
        size_t start = position;
        while (position < input.length() && 
               (std::isalnum(input[position]) || input[position] == '_')) {
            position++;
        }
        
        std::string identifier = input.substr(start, position - start);
        std::string upperIdentifier = identifier;
        std::transform(upperIdentifier.begin(), upperIdentifier.end(), 
                      upperIdentifier.begin(), ::toupper);
        
        // Check for keywords
        if (upperIdentifier == "SELECT") {
            return Token(TokenType::SELECT, identifier, start);
        } else if (upperIdentifier == "FROM") {
            return Token(TokenType::FROM, identifier, start);
        } else if (upperIdentifier == "WHERE") {
            return Token(TokenType::WHERE, identifier, start);
        } else if (upperIdentifier == "AND") {
            return Token(TokenType::AND, identifier, start);
        } else if (upperIdentifier == "OR") {
            return Token(TokenType::OR, identifier, start);
        } else if (upperIdentifier == "CREATE") {
            return Token(TokenType::CREATE, identifier, start);
        } else if (upperIdentifier == "DROP") {
            return Token(TokenType::DROP, identifier, start);
        } else if (upperIdentifier == "TABLE") {
            return Token(TokenType::TABLE, identifier, start);
        } else if (upperIdentifier == "INSERT") {
            return Token(TokenType::INSERT, identifier, start);
        } else if (upperIdentifier == "INTO") {
            return Token(TokenType::INTO, identifier, start);
        } else if (upperIdentifier == "VALUES") {
            return Token(TokenType::VALUES, identifier, start);
        } else if (upperIdentifier == "INT" || upperIdentifier == "INTEGER") {
            return Token(TokenType::INT, identifier, start);
        } else if (upperIdentifier == "DOUBLE" || upperIdentifier == "FLOAT" || upperIdentifier == "REAL") {
            return Token(TokenType::DOUBLE, identifier, start);
        } else if (upperIdentifier == "STRING" || upperIdentifier == "VARCHAR" || upperIdentifier == "TEXT") {
            return Token(TokenType::STRING, identifier, start);
        } else if (upperIdentifier == "BOOLEAN" || upperIdentifier == "BOOL") {
            return Token(TokenType::BOOLEAN, identifier, start);
        } else if (upperIdentifier == "NOT") {
            return Token(TokenType::NOT, identifier, start);
        } else if (upperIdentifier == "NULL") {
            return Token(TokenType::NULL_TOKEN, identifier, start);
        } else if (upperIdentifier == "UNIQUE") {
            return Token(TokenType::UNIQUE, identifier, start);
        } else if (upperIdentifier == "PRIMARY") {
            return Token(TokenType::PRIMARY, identifier, start);
        } else if (upperIdentifier == "KEY") {
            return Token(TokenType::KEY, identifier, start);
        } else {
            return Token(TokenType::IDENTIFIER, identifier, start);
        }
    }
    
    Token readNumber() {
        size_t start = position;
        bool hasDecimal = false;
        
        while (position < input.length()) {
            if (std::isdigit(input[position])) {
                position++;
            } else if (input[position] == '.' && !hasDecimal) {
                hasDecimal = true;
                position++;
            } else {
                break;
            }
        }
        
        return Token(TokenType::NUMBER, input.substr(start, position - start), start);
    }
    
    Token readStringLiteral() {
        size_t start = position;
        position++; // Skip opening quote
        
        while (position < input.length() && input[position] != '\'') {
            if (input[position] == '\\' && position + 1 < input.length()) {
                position += 2; // Skip escaped character
            } else {
                position++;
            }
        }
        
        if (position >= input.length()) {
            return Token(TokenType::ERROR, "Unterminated string", start);
        }
        
        position++; // Skip closing quote
        return Token(TokenType::STRING_LITERAL, 
                    input.substr(start + 1, position - start - 2), start);
    }
};

} // namespace parallaxdb 