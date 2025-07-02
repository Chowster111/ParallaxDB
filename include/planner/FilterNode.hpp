#pragma once

#include "QueryPlan.hpp"
#include "../storage/Table.hpp"
#include <functional>
#include <iostream>
#include <variant>

namespace parallaxdb {

class FilterNode : public QueryPlanNode {
public:
    FilterNode(std::unique_ptr<QueryPlanNode> child,
               std::function<bool(const Row&)> predicate)
        : child(std::move(child)), predicate(predicate) {}

    void execute() override {
        // The child node should produce rows (we’ll do this naively for now)
        TableScanNode* scan = dynamic_cast<TableScanNode*>(child.get());
        if (scan == nullptr) {
            std::cerr << "FilterNode currently supports only TableScanNode child" << std::endl;
            return;
        }

        const Table& table = scan->getTable();
        for (const auto& row : table.getRows()) {
            if (predicate(row)) {
                for (const auto& val : row.values) {
                    std::cout << val << " ";
                }
                std::cout << std::endl;
            }
        }
    }

private:
    std::unique_ptr<QueryPlanNode> child;
    std::function<bool(const Row&)> predicate;
};

} // namespace parallaxdb
