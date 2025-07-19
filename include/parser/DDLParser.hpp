#pragma once

#include "Tokenizer.hpp"
#include "Token.hpp"
#include "../storage/Database.hpp"
#include "../types/Common.hpp"
#include <memory>
#include <stdexcept>

namespace parallaxdb {

struct CreateTableStatement {
    std::string tableName;
    Schema schema;
    
    CreateTableStatement() : tableName(""), schema("") {}
    CreateTableStatement(const std::string& name, const Schema& s) : tableName(name), schema(s) {}
};

struct DropTableStatement {
    std::string tableName;
    
    DropTableStatement() : tableName("") {}
    DropTableStatement(const std::string& name) : tableName(name) {}
};

class DDLParser {
public:
    static std::unique_ptr<CreateTableStatement> parseCreateTable(const std::string& query);
    static std::unique_ptr<DropTableStatement> parseDropTable(const std::string& query);
    
private:
    static Schema parseTableSchema(const std::vector<Token>& tokens, size_t& pos);
    static Column parseColumnDefinition(const std::vector<Token>& tokens, size_t& pos);
    static std::vector<Constraint> parseConstraints(const std::vector<Token>& tokens, size_t& pos);
    static Constraint parseConstraint(const std::vector<Token>& tokens, size_t& pos);
};

} // namespace parallaxdb 