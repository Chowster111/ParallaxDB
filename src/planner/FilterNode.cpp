#include "../../include/planner/FilterNode.hpp"

namespace parallaxdb {

FilterNode::FilterNode(std::unique_ptr<QueryPlanNode> child, std::unique_ptr<Expression> expr, const Table& table)
    : child(std::move(child)), expr(std::move(expr)), table(table) {}

} // namespace parallaxdb 