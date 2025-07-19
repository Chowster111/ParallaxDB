#pragma once

#include "Table.hpp"
#include "../types/Common.hpp"
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace parallaxdb {

class Database {
public:
    Database() = default;
    
    // Table management
    void createTable(const std::string& tableName, const Schema& schema);
    void dropTable(const std::string& tableName);
    bool tableExists(const std::string& tableName) const;
    const Table* getTable(const std::string& tableName) const;
    Table* getTable(const std::string& tableName);
    
    // Schema management
    Schema* getSchema(const std::string& tableName);
    const Schema* getSchema(const std::string& tableName) const;
    
    // Data manipulation
    void insertInto(const std::string& tableName, const Row& row);
    void insertInto(const std::string& tableName, const std::vector<Value>& values);
    
    // Utility methods
    std::vector<std::string> getTableNames() const;
    size_t getTableCount() const { return tables.size(); }
    
    // Clear all data (for testing)
    void clear() { tables.clear(); }

private:
    std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};

} // namespace parallaxdb 