#include "../../include/parser/DMLParser.hpp"
#include <algorithm>

namespace parallaxdb {

std::unique_ptr<InsertStatement> DMLParser::parseInsert(const std::string& query) {
    Tokenizer tokenizer(query);
    auto tokens = tokenizer.tokenize();
    size_t pos = 0;
    
    // Parse INSERT INTO
    if (pos >= tokens.size() || tokens[pos].type != TokenType::INSERT) {
        throw std::runtime_error("Expected INSERT [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    if (pos >= tokens.size() || tokens[pos].type != TokenType::INTO) {
        throw std::runtime_error("Expected INTO [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    // Parse table name
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected table name [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    std::string tableName = tokens[pos].value;
    pos++;
    
    auto result = std::make_unique<InsertStatement>();
    result->tableName = tableName;
    
    // Parse optional column list
    if (pos < tokens.size() && tokens[pos].type == TokenType::LEFT_PAREN) {
        result->columns = parseColumnList(tokens, pos);
    }
    
    // Parse VALUES
    if (pos >= tokens.size() || tokens[pos].type != TokenType::VALUES) {
        throw std::runtime_error("Expected VALUES [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    // Parse value lists
    while (pos < tokens.size()) {
        if (tokens[pos].type == TokenType::LEFT_PAREN) {
            result->values.push_back(parseValueList(tokens, pos));
        } else {
            break;
        }
        
        // Check for comma separator between value lists
        if (pos < tokens.size() && tokens[pos].type == TokenType::COMMA) {
            pos++;
        } else {
            break;
        }
    }
    
    return result;
}

std::vector<std::string> DMLParser::parseColumnList(const std::vector<Token>& tokens, size_t& pos) {
    std::vector<std::string> columns;
    
    // Parse opening parenthesis
    if (pos >= tokens.size() || tokens[pos].type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    while (pos < tokens.size()) {
        if (tokens[pos].type == TokenType::RIGHT_PAREN) {
            pos++;
            break;
        }
        
        if (tokens[pos].type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected column name [pos=" + std::to_string(tokens[pos].position) + "]");
        }
        
        columns.push_back(tokens[pos].value);
        pos++;
        
        if (pos < tokens.size() && tokens[pos].type == TokenType::COMMA) {
            pos++;
        } else if (pos < tokens.size() && tokens[pos].type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ',' or ')' [pos=" + std::to_string(tokens[pos].position) + "]");
        }
    }
    
    return columns;
}

std::vector<Value> DMLParser::parseValueList(const std::vector<Token>& tokens, size_t& pos) {
    std::vector<Value> values;
    
    // Parse opening parenthesis
    if (pos >= tokens.size() || tokens[pos].type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    while (pos < tokens.size()) {
        if (tokens[pos].type == TokenType::RIGHT_PAREN) {
            pos++;
            break;
        }
        
        values.push_back(parseValue(tokens, pos));
        
        if (pos < tokens.size() && tokens[pos].type == TokenType::COMMA) {
            pos++;
        } else if (pos < tokens.size() && tokens[pos].type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ',' or ')' [pos=" + std::to_string(tokens[pos].position) + "]");
        }
    }
    
    return values;
}

Value DMLParser::parseValue(const std::vector<Token>& tokens, size_t& pos) {
    if (pos >= tokens.size()) {
        throw std::runtime_error("Expected value [pos=" + std::to_string(pos) + "]");
    }
    
    if (tokens[pos].type == TokenType::NUMBER) {
        std::string numStr = tokens[pos].value;
        pos++;
        
        // Try to parse as integer first, then as double
        try {
            return std::stoi(numStr);
        } catch (...) {
            try {
                return std::stod(numStr);
            } catch (...) {
                throw std::runtime_error("Invalid number: " + numStr + " [pos=" + std::to_string(tokens[pos-1].position) + "]");
            }
        }
    } else if (tokens[pos].type == TokenType::STRING_LITERAL) {
        std::string str = tokens[pos].value;
        pos++;
        return str;
    } else if (tokens[pos].type == TokenType::IDENTIFIER) {
        std::string identifier = tokens[pos].value;
        std::transform(identifier.begin(), identifier.end(), identifier.begin(), ::toupper);
        
        if (identifier == "NULL") {
            pos++;
            return std::nullptr_t{};
        } else {
            throw std::runtime_error("Unknown identifier: " + identifier + " [pos=" + std::to_string(tokens[pos].position) + "]");
        }
    } else {
        throw std::runtime_error("Expected value [pos=" + std::to_string(tokens[pos].position) + "]");
    }
}

} // namespace parallaxdb 