#include "../../include/planner/QueryPlan.hpp"

namespace parallaxdb {

TableScanNode::TableScanNode(const Table& table, const std::vector<std::string>& selectedColumns)
    : table(table), selectedColumns(selectedColumns) {}

const Table& TableScanNode::getTable() const { return table; }
const std::vector<std::string>& TableScanNode::getSelectedColumns() const { return selectedColumns; }

} // namespace parallaxdb 