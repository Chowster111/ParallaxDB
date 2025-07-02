#pragma once

#include "../storage/Table.hpp"
#include <string>
#include <vector>

namespace parallaxdb {

class QueryPlanNode {
public:
    virtual ~QueryPlanNode() = default;
    virtual void execute() = 0;
};

// Simple scan node
class TableScanNode : public QueryPlanNode {
public:
    TableScanNode(const Table& table) : table(table) {}

    void execute() override {
        for (const auto& row : table.getRows()) {
            // For now, just print
            for (const auto& val : row.values) {
                std::cout << val << " ";

            }
            std::cout << std::endl;
        }
    }

private:
    const Table& table;
};

} // namespace parallaxdb
