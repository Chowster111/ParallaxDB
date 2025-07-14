#pragma once

#include "../storage/Table.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <variant>

namespace parallaxdb {

// Helper function to print variant values
inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    std::visit([&os](const auto& v) { os << v; }, val);
    return os;
}

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

    const Table& getTable() const { return table; }

private:
    const Table& table;
};

} // namespace parallaxdb
