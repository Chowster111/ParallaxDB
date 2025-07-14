#pragma once

#include "../storage/Table.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <variant>

namespace parallaxdb {

// Helper function to print variant values
inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    std::visit([&os](const auto& v) { os << v; }, val);
    return os;
}

class QueryPlanNode {
public:
    virtual ~QueryPlanNode() = default;
    virtual void execute() = 0;
};

// Simple scan node
class TableScanNode : public QueryPlanNode {
public:
    TableScanNode(const Table& table, const std::vector<std::string>& selectedColumns = {})
        : table(table), selectedColumns(selectedColumns) {}

    void execute() override {
        const auto& columns = table.getColumns();
        std::vector<int> colIndices;
        if (!selectedColumns.empty()) {
            // Map selected column names to indices
            for (const auto& colName : selectedColumns) {
                for (size_t i = 0; i < columns.size(); ++i) {
                    if (columns[i].name == colName) {
                        colIndices.push_back(i);
                        break;
                    }
                }
            }
        }
        for (const auto& row : table.getRows()) {
            if (!selectedColumns.empty()) {
                for (size_t j = 0; j < colIndices.size(); ++j) {
                    int idx = colIndices[j];
                    if (idx >= 0 && idx < (int)row.values.size()) {
                        std::cout << row.values[idx] << " ";
                    }
                }
            } else {
                for (const auto& val : row.values) {
                    std::cout << val << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    const Table& getTable() const { return table; }
    const std::vector<std::string>& getSelectedColumns() const { return selectedColumns; }

private:
    const Table& table;
    std::vector<std::string> selectedColumns;
};

} // namespace parallaxdb
