#pragma once

#include "../planner/QueryPlan.hpp"
#include "../storage/Table.hpp"
#include <string>
#include <memory>

namespace parallaxdb {

class Parser {
public:
    static std::unique_ptr<QueryPlanNode> parse(const std::string& query, const Table& table) {
        // VERY dummy: just always return a TableScanNode
        return std::make_unique<TableScanNode>(table);
    }
};

} // namespace parallaxdb
