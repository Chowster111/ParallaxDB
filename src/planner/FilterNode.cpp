#include "../../include/planner/FilterNode.hpp"
#include "../../include/planner/QueryPlan.hpp"
#include "../../include/types/Common.hpp"
#include <iostream>

namespace parallaxdb {

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