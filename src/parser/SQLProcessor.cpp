#include "../../include/parser/SQLProcessor.hpp"
#include "../../include/executor/QueryExecutor.hpp"
#include <iostream>
#include <algorithm>

namespace parallaxdb {

StatementType SQLProcessor::getStatementType(const std::string& query) {
    std::string upperQuery = query;
    std::transform(upperQuery.begin(), upperQuery.end(), upperQuery.begin(), ::toupper);
    
    // Remove leading whitespace
    size_t start = upperQuery.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return StatementType::UNKNOWN;
    
    upperQuery = upperQuery.substr(start);
    
    if (upperQuery.substr(0, 6) == "SELECT") {
        return StatementType::SELECT;
    } else if (upperQuery.substr(0, 6) == "INSERT") {
        return StatementType::INSERT;
    } else if (upperQuery.substr(0, 6) == "CREATE") {
        return StatementType::CREATE_TABLE;
    } else if (upperQuery.substr(0, 4) == "DROP") {
        return StatementType::DROP_TABLE;
    }
    
    return StatementType::UNKNOWN;
}

std::unique_ptr<QueryPlanNode> SQLProcessor::processSelect(const std::string& query, Database& db) {
    // For now, we'll assume the table exists and use the first table
    // In a real implementation, you'd parse the table name from the query
    auto tableNames = db.getTableNames();
    if (tableNames.empty()) {
        std::cout << "No tables available for SELECT" << std::endl;
        return nullptr;
    }
    
    const Table* table = db.getTable(tableNames[0]);
    if (!table) {
        std::cout << "Table not found: " << tableNames[0] << std::endl;
        return nullptr;
    }
    
    return SQLParser::parse(query, *table);
}

void SQLProcessor::processInsert(const std::string& query, Database& db) {
    try {
        auto insertStmt = DMLParser::parseInsert(query);
        
        if (!db.tableExists(insertStmt->tableName)) {
            std::cout << "Table '" << insertStmt->tableName << "' does not exist" << std::endl;
            return;
        }
        
        const Table* table = db.getTable(insertStmt->tableName);
        if (!table) {
            std::cout << "Error accessing table: " << insertStmt->tableName << std::endl;
            return;
        }
        
        // Insert each row
        for (const auto& values : insertStmt->values) {
            try {
                db.insertInto(insertStmt->tableName, values);
                std::cout << "Inserted row into " << insertStmt->tableName << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error inserting row: " << e.what() << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Parse error: " << e.what() << std::endl;
    }
}

void SQLProcessor::processCreateTable(const std::string& query, Database& db) {
    try {
        auto createStmt = DDLParser::parseCreateTable(query);
        
        if (db.tableExists(createStmt->tableName)) {
            std::cout << "Table '" << createStmt->tableName << "' already exists" << std::endl;
            return;
        }
        
        db.createTable(createStmt->tableName, createStmt->schema);
        std::cout << "Created table '" << createStmt->tableName << "' with " 
                  << createStmt->schema.columns.size() << " columns" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Parse error: " << e.what() << std::endl;
    }
}

void SQLProcessor::processDropTable(const std::string& query, Database& db) {
    try {
        auto dropStmt = DDLParser::parseDropTable(query);
        
        if (!db.tableExists(dropStmt->tableName)) {
            std::cout << "Table '" << dropStmt->tableName << "' does not exist" << std::endl;
            return;
        }
        
        db.dropTable(dropStmt->tableName);
        std::cout << "Dropped table '" << dropStmt->tableName << "'" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Parse error: " << e.what() << std::endl;
    }
}

void SQLProcessor::processStatement(const std::string& query, Database& db) {
    StatementType type = getStatementType(query);
    
    switch (type) {
        case StatementType::SELECT: {
            auto plan = processSelect(query, db);
            if (plan) {
                plan->execute();
            }
            break;
        }
        case StatementType::INSERT:
            processInsert(query, db);
            break;
        case StatementType::CREATE_TABLE:
            processCreateTable(query, db);
            break;
        case StatementType::DROP_TABLE:
            processDropTable(query, db);
            break;
        case StatementType::UNKNOWN:
            std::cout << "Unknown statement type" << std::endl;
            break;
    }
}

void SQLProcessor::printResults(const std::vector<Row>& results, const std::vector<std::string>& columns) {
    if (results.empty()) {
        std::cout << "No results" << std::endl;
        return;
    }
    
    // Print header
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) std::cout << " | ";
        std::cout << columns[i];
    }
    std::cout << std::endl;
    
    // Print separator
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i > 0) std::cout << " | ";
        std::cout << std::string(columns[i].length(), '-');
    }
    std::cout << std::endl;
    
    // Print rows
    for (const auto& row : results) {
        for (size_t i = 0; i < row.values.size(); ++i) {
            if (i > 0) std::cout << " | ";
            std::visit([](const auto& v) { std::cout << v; }, row.values[i]);
        }
        std::cout << std::endl;
    }
}

} // namespace parallaxdb 