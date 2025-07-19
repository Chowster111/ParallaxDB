#include "../../include/parser/DDLParser.hpp"
#include <algorithm>

namespace parallaxdb {

std::unique_ptr<CreateTableStatement> DDLParser::parseCreateTable(const std::string& query) {
    Tokenizer tokenizer(query);
    auto tokens = tokenizer.tokenize();
    size_t pos = 0;
    
    // Parse CREATE TABLE
    if (pos >= tokens.size() || tokens[pos].type != TokenType::CREATE) {
        throw std::runtime_error("Expected CREATE [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    if (pos >= tokens.size() || tokens[pos].type != TokenType::TABLE) {
        throw std::runtime_error("Expected TABLE [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    // Parse table name
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected table name [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    std::string tableName = tokens[pos].value;
    pos++;
    
    // Parse opening parenthesis
    if (pos >= tokens.size() || tokens[pos].type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    // Parse schema
    Schema schema = parseTableSchema(tokens, pos);
    schema.tableName = tableName;
    
    // Parse closing parenthesis
    if (pos >= tokens.size() || tokens[pos].type != TokenType::RIGHT_PAREN) {
        throw std::runtime_error("Expected ')' [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    auto result = std::make_unique<CreateTableStatement>();
    result->tableName = tableName;
    result->schema = schema;
    
    return result;
}

std::unique_ptr<DropTableStatement> DDLParser::parseDropTable(const std::string& query) {
    Tokenizer tokenizer(query);
    auto tokens = tokenizer.tokenize();
    size_t pos = 0;
    
    // Parse DROP TABLE
    if (pos >= tokens.size() || tokens[pos].type != TokenType::DROP) {
        throw std::runtime_error("Expected DROP [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    if (pos >= tokens.size() || tokens[pos].type != TokenType::TABLE) {
        throw std::runtime_error("Expected TABLE [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    pos++;
    
    // Parse table name
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected table name [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    std::string tableName = tokens[pos].value;
    pos++;
    
    auto result = std::make_unique<DropTableStatement>();
    result->tableName = tableName;
    
    return result;
}

Schema DDLParser::parseTableSchema(const std::vector<Token>& tokens, size_t& pos) {
    Schema schema("");
    
    while (pos < tokens.size()) {
        if (tokens[pos].type == TokenType::RIGHT_PAREN) {
            break;
        }
        
        Column column = parseColumnDefinition(tokens, pos);
        schema.columns.push_back(column);
        
        // Check for comma separator
        if (pos < tokens.size() && tokens[pos].type == TokenType::COMMA) {
            pos++;
        } else if (pos < tokens.size() && tokens[pos].type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ',' or ')' [pos=" + std::to_string(tokens[pos].position) + "]");
        }
    }
    
    return schema;
}

Column DDLParser::parseColumnDefinition(const std::vector<Token>& tokens, size_t& pos) {
    // Parse column name
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected column name [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    std::string columnName = tokens[pos].value;
    pos++;
    
    // Parse data type
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected data type [pos=" + std::to_string(tokens[pos].position) + "]");
    }
    std::string typeName = tokens[pos].value;
    DataType dataType = DataValidator::parseTypeName(typeName);
    pos++;
    
    Column column(columnName, dataType);
    
    // Parse constraints (optional)
    while (pos < tokens.size() && tokens[pos].type == TokenType::IDENTIFIER) {
        std::vector<Constraint> constraints = parseConstraints(tokens, pos);
        column.constraints.insert(column.constraints.end(), constraints.begin(), constraints.end());
    }
    
    return column;
}

std::vector<Constraint> DDLParser::parseConstraints(const std::vector<Token>& tokens, size_t& pos) {
    std::vector<Constraint> constraints;
    
    while (pos < tokens.size() && tokens[pos].type == TokenType::IDENTIFIER) {
        std::string constraintName = tokens[pos].value;
        std::transform(constraintName.begin(), constraintName.end(), constraintName.begin(), ::toupper);
        
        if (constraintName == "NOT" && pos + 1 < tokens.size() && 
            tokens[pos + 1].type == TokenType::IDENTIFIER && 
            tokens[pos + 1].value == "NULL") {
            pos += 2;
            constraints.emplace_back(Constraint::NOT_NULL, "NOT_NULL");
        } else if (constraintName == "UNIQUE") {
            pos++;
            constraints.emplace_back(Constraint::UNIQUE, "UNIQUE");
        } else if (constraintName == "PRIMARY" && pos + 1 < tokens.size() && 
                   tokens[pos + 1].type == TokenType::IDENTIFIER && 
                   tokens[pos + 1].value == "KEY") {
            pos += 2;
            constraints.emplace_back(Constraint::PRIMARY_KEY, "PRIMARY_KEY");
        } else {
            // Unknown constraint, skip it
            pos++;
        }
    }
    
    return constraints;
}

} // namespace parallaxdb 