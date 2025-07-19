#pragma once

#include "Tokenizer.hpp"
#include "Token.hpp"
#include "../storage/Database.hpp"
#include "../types/Common.hpp"
#include <memory>
#include <stdexcept>

namespace parallaxdb {

struct InsertStatement {
    std::string tableName;
    std::vector<std::string> columns;  // Optional column list
    std::vector<std::vector<Value>> values;  // Multiple rows
    
    InsertStatement() : tableName("") {}
    InsertStatement(const std::string& name) : tableName(name) {}
};

class DMLParser {
public:
    static std::unique_ptr<InsertStatement> parseInsert(const std::string& query);
    
private:
    static std::vector<std::string> parseColumnList(const std::vector<Token>& tokens, size_t& pos);
    static std::vector<Value> parseValueList(const std::vector<Token>& tokens, size_t& pos);
    static Value parseValue(const std::vector<Token>& tokens, size_t& pos);
};

} // namespace parallaxdb 