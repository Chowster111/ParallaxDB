#pragma once
#include "../planner/QueryPlan.hpp"
#include "../planner/FilterNode.hpp"
#include "../types/Common.hpp"
#include <iostream>
#include <variant>
#include <memory>

namespace parallaxdb {



// QueryExecutor class declaration
class QueryExecutor {
public:
    static std::vector<Row> execute(const QueryPlanNode& plan);
};

} // namespace parallaxdb 