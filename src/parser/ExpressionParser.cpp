#include "../../include/parser/ExpressionParser.hpp"
#include "../../include/parser/Expression.hpp"
#include <stdexcept>

namespace parallaxdb {

std::unique_ptr<Expression> ExpressionParser::parseWhereExpression(const std::vector<Token>& tokens, size_t& pos) {
    return parseOr(tokens, pos);
}
std::unique_ptr<Expression> ExpressionParser::parseOr(const std::vector<Token>& tokens, size_t& pos) {
    auto left = parseAnd(tokens, pos);
    while (pos < tokens.size() && tokens[pos].type == TokenType::OR) {
        pos++;
        auto right = parseAnd(tokens, pos);
        left = std::make_unique<LogicalExpr>("OR", std::move(left), std::move(right));
    }
    return left;
}
std::unique_ptr<Expression> ExpressionParser::parseAnd(const std::vector<Token>& tokens, size_t& pos) {
    auto left = parsePrimary(tokens, pos);
    while (pos < tokens.size() && tokens[pos].type == TokenType::AND) {
        pos++;
        auto right = parsePrimary(tokens, pos);
        left = std::make_unique<LogicalExpr>("AND", std::move(left), std::move(right));
    }
    return left;
}
std::unique_ptr<Expression> ExpressionParser::parsePrimary(const std::vector<Token>& tokens, size_t& pos) {
    if (tokens[pos].type == TokenType::LEFT_PAREN) {
        pos++;
        auto expr = parseWhereExpression(tokens, pos);
        if (tokens[pos].type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ')' [pos=" + std::to_string(tokens[pos].position) + "]");
        }
        pos++;
        return std::make_unique<ParenExpr>(std::move(expr));
    }
    // Parse comparison: col op value
    if (tokens[pos].type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected column name in WHERE clause [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    std::string col = tokens[pos].value;
    pos++;
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected operator in WHERE clause [pos=" + std::to_string(pos) + "]");
    }
    std::string op;
    if (tokens[pos].type == TokenType::GREATER_THAN) op = ">";
    else if (tokens[pos].type == TokenType::LESS_THAN) op = "<";
    else if (tokens[pos].type == TokenType::EQUALS) op = "=";
    else if (tokens[pos].type == TokenType::GREATER_EQUAL) op = ">=";
    else if (tokens[pos].type == TokenType::LESS_EQUAL) op = "<=";
    else if (tokens[pos].type == TokenType::NOT_EQUALS) op = "!=";
    else throw std::runtime_error("Expected comparison operator [pos=" + std::to_string(tokens[pos].position) + "]");
    pos++;
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected value in WHERE clause [pos=" + std::to_string(pos) + "]");
    }
    Value val;
    if (tokens[pos].type == TokenType::NUMBER) {
        try {
            val = std::stoi(tokens[pos].value);
        } catch (...) {
            try {
                val = std::stod(tokens[pos].value);
            } catch (...) {
                throw std::runtime_error("Invalid number: " + tokens[pos].value + " [pos=" + std::to_string(tokens[pos].position) + "]");
            }
        }
    } else if (tokens[pos].type == TokenType::STRING_LITERAL) {
        val = tokens[pos].value;
    } else {
        throw std::runtime_error("Expected value [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    return std::make_unique<ComparisonExpr>(col, op, val);
}

} // namespace parallaxdb 