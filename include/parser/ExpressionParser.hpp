#pragma once
#include "Expression.hpp"
#include "Tokenizer.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>
#include <string>

namespace parallaxdb {

// ExpressionParser: Handles parsing of WHERE clause expressions (with precedence and parentheses)
class ExpressionParser {
public:
    // Parses a WHERE clause expression from tokens starting at pos
    static std::unique_ptr<Expression> parseWhereExpression(const std::vector<Token>& tokens, size_t& pos);

private:
    static std::unique_ptr<Expression> parseOr(const std::vector<Token>& tokens, size_t& pos);
    static std::unique_ptr<Expression> parseAnd(const std::vector<Token>& tokens, size_t& pos);
    static std::unique_ptr<Expression> parsePrimary(const std::vector<Token>& tokens, size_t& pos);
};

} // namespace parallaxdb 