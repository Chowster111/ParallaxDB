#pragma once
#include "../planner/QueryPlan.hpp"
#include "../planner/FilterNode.hpp"
#include "../types/Common.hpp"
#include <iostream>
#include <variant>
#include <memory>

namespace parallaxdb {

// Helper function to print variant values
inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    std::visit([&os](const auto& v) { os << v; }, val);
    return os;
}

// TableScanNode implementation
TableScanNode::TableScanNode(const Table& table, const std::vector<std::string>& selectedColumns)
    : table(table), selectedColumns(selectedColumns) {}

void TableScanNode::execute() {
    const auto& columns = table.getColumns();
    std::vector<int> colIndices;
    if (!selectedColumns.empty()) {
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

const Table& TableScanNode::getTable() const { return table; }
const std::vector<std::string>& TableScanNode::getSelectedColumns() const { return selectedColumns; }

// FilterNode implementation
FilterNode::FilterNode(std::unique_ptr<QueryPlanNode> child, std::unique_ptr<Expression> expr, const Table& table)
    : child(std::move(child)), expr(std::move(expr)), table(table) {}

void FilterNode::execute() {
    TableScanNode* scan = dynamic_cast<TableScanNode*>(child.get());
    if (scan == nullptr) {
        std::cerr << "FilterNode currently supports only TableScanNode child" << std::endl;
        return;
    }
    for (const auto& row : table.getRows()) {
        if (expr->evaluate(row, table)) {
            for (const auto& val : row.values) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }
}

} // namespace parallaxdb 