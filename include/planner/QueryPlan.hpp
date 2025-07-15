#pragma once

#include "../storage/Table.hpp"
#include "../types/Common.hpp"
#include <string>
#include <vector>
#include <memory>

namespace parallaxdb {

class QueryPlanNode {
public:
    virtual ~QueryPlanNode() = default;
    virtual void execute() = 0;
};

class TableScanNode : public QueryPlanNode {
public:
    TableScanNode(const Table& table, const std::vector<std::string>& selectedColumns = {});
    void execute() override;
    const Table& getTable() const;
    const std::vector<std::string>& getSelectedColumns() const;
private:
    const Table& table;
    std::vector<std::string> selectedColumns;
};

} // namespace parallaxdb
