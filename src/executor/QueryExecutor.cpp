#include "../../include/executor/QueryExecutor.hpp"
#include "../../include/planner/QueryPlan.hpp"
#include "../../include/planner/FilterNode.hpp"
#include <iostream>

namespace parallaxdb {

std::vector<Row> QueryExecutor::execute(const QueryPlanNode& plan) {
    std::vector<Row> results;
    
    // For now, we'll just call the execute method which prints to stdout
    // In a real implementation, we'd collect the results
    const_cast<QueryPlanNode&>(plan).execute();
    
    return results;
}

} // namespace parallaxdb 