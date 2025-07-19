#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
#include "../types/Common.hpp"

namespace parallaxdb {

class Table {
public:
    Table(const std::string& name, const Schema& schema)
        : name(name), schema(schema) {}

    // Legacy constructor for backward compatibility
    Table(const std::string& name, const std::vector<Column>& columns)
        : name(name), schema(name) {
        schema.columns = columns;
    }

    void insertRow(const Row& row) {
        if (!DataValidator::validateRow(row, schema)) {
            throw std::runtime_error("Row validation failed for table: " + name);
        }
        rows.push_back(row);
    }

    void insertRow(const std::vector<Value>& values) {
        Row row{values};
        insertRow(row);
    }

    const std::vector<Column>& getColumns() const {
        return schema.columns;
    }

    const std::vector<Row>& getRows() const {
        return rows;
    }

    const std::string& getName() const {
        return name;
    }

    const Schema& getSchema() const {
        return schema;
    }

    // Schema management
    void setSchema(const Schema& newSchema) {
        schema = newSchema;
    }

    // Data validation
    bool validateRow(const Row& row) const {
        return DataValidator::validateRow(row, schema);
    }

    // Get column index by name
    int getColumnIndex(const std::string& columnName) const {
        for (size_t i = 0; i < schema.columns.size(); ++i) {
            if (schema.columns[i].name == columnName) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    // Get column by name
    const Column* getColumn(const std::string& columnName) const {
        int index = getColumnIndex(columnName);
        return (index >= 0) ? &schema.columns[index] : nullptr;
    }

private:
    std::string name;
    Schema schema;
    std::vector<Row> rows;
};

} // namespace parallaxdb
