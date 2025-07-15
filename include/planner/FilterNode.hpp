#pragma once

#include "QueryPlan.hpp"
#include "../storage/Table.hpp"
#include "../parser/Expression.hpp"
#include "../types/Common.hpp"
#include <memory>

namespace parallaxdb {

class FilterNode : public QueryPlanNode {
public:
    FilterNode(std::unique_ptr<QueryPlanNode> child,
               std::unique_ptr<Expression> expr,
               const Table& table);
    void execute() override;
private:
    std::unique_ptr<QueryPlanNode> child;
    std::unique_ptr<Expression> expr;
    const Table& table;
};

} // namespace parallaxdb
