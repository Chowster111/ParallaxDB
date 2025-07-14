#pragma once

#include "QueryPlan.hpp"
#include "../storage/Table.hpp"
#include "../parser/Expression.hpp"
#include <functional>
#include <iostream>
#include <variant>
#include <memory>

namespace parallaxdb {

class FilterNode : public QueryPlanNode {
public:
    FilterNode(std::unique_ptr<QueryPlanNode> child,
               std::unique_ptr<Expression> expr,
               const Table& table)
        : child(std::move(child)), expr(std::move(expr)), table(table) {}

    void execute() override {
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

private:
    std::unique_ptr<QueryPlanNode> child;
    std::unique_ptr<Expression> expr;
    const Table& table;
};

} // namespace parallaxdb
