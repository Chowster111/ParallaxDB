#include "../../include/planner/QueryPlan.hpp"
#include "../../include/types/Common.hpp"
#include <iostream>

namespace parallaxdb {

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

} // namespace parallaxdb 