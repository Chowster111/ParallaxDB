#include "../../include/storage/Database.hpp"
#include <algorithm>

namespace parallaxdb {

void Database::createTable(const std::string& tableName, const Schema& schema) {
    if (tableExists(tableName)) {
        throw std::runtime_error("Table '" + tableName + "' already exists");
    }
    
    Schema newSchema = schema;
    newSchema.tableName = tableName;
    tables[tableName] = std::make_unique<Table>(tableName, newSchema);
}

void Database::dropTable(const std::string& tableName) {
    if (!tableExists(tableName)) {
        throw std::runtime_error("Table '" + tableName + "' does not exist");
    }
    
    tables.erase(tableName);
}

bool Database::tableExists(const std::string& tableName) const {
    return tables.find(tableName) != tables.end();
}

const Table* Database::getTable(const std::string& tableName) const {
    auto it = tables.find(tableName);
    return (it != tables.end()) ? it->second.get() : nullptr;
}

Table* Database::getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    return (it != tables.end()) ? it->second.get() : nullptr;
}

Schema* Database::getSchema(const std::string& tableName) {
    Table* table = getTable(tableName);
    return table ? &const_cast<Schema&>(table->getSchema()) : nullptr;
}

const Schema* Database::getSchema(const std::string& tableName) const {
    const Table* table = getTable(tableName);
    return table ? &table->getSchema() : nullptr;
}

void Database::insertInto(const std::string& tableName, const Row& row) {
    Table* table = getTable(tableName);
    if (!table) {
        throw std::runtime_error("Table '" + tableName + "' does not exist");
    }
    
    table->insertRow(row);
}

void Database::insertInto(const std::string& tableName, const std::vector<Value>& values) {
    Table* table = getTable(tableName);
    if (!table) {
        throw std::runtime_error("Table '" + tableName + "' does not exist");
    }
    
    table->insertRow(values);
}

std::vector<std::string> Database::getTableNames() const {
    std::vector<std::string> names;
    names.reserve(tables.size());
    
    for (const auto& pair : tables) {
        names.push_back(pair.first);
    }
    
    return names;
}

} // namespace parallaxdb 