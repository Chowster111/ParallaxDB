#pragma once

#include "SQLParser.hpp"
#include "DDLParser.hpp"
#include "DMLParser.hpp"
#include "../storage/Database.hpp"
#include <memory>
#include <string>

namespace parallaxdb {

enum class StatementType {
    SELECT,
    INSERT,
    CREATE_TABLE,
    DROP_TABLE,
    UNKNOWN
};

class SQLProcessor {
public:
    static StatementType getStatementType(const std::string& query);
    
    // Process different types of statements
    static std::unique_ptr<QueryPlanNode> processSelect(const std::string& query, Database& db);
    static void processInsert(const std::string& query, Database& db);
    static void processCreateTable(const std::string& query, Database& db);
    static void processDropTable(const std::string& query, Database& db);
    
    // Main entry point
    static void processStatement(const std::string& query, Database& db);
    
private:
    static void printResults(const std::vector<Row>& results, const std::vector<std::string>& columns);
};

} // namespace parallaxdb 