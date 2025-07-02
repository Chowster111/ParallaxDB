#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

namespace parallaxdb {

using Value = std::string;

struct Row {
    std::vector<Value> values;
};

struct Column {
    std::string name;
    enum Type { INT, DOUBLE, STRING } type;
};

class Table {
public:
    Table(const std::string& name, const std::vector<Column>& columns)
        : name(name), columns(columns) {}

    void insertRow(const Row& row) {
        rows.push_back(row);
    }

    const std::vector<Column>& getColumns() const {
        return columns;
    }

    const std::vector<Row>& getRows() const {
        return rows;
    }

    const std::string& getName() const {
        return name;
    }

private:
    std::string name;
    std::vector<Column> columns;
    std::vector<Row> rows;
};

} // namespace parallaxdb
