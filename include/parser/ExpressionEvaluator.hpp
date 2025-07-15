#pragma once
#include <variant>
#include <string>
#include <memory>

namespace parallaxdb {

struct ComparisonExpr;
struct LogicalExpr;
struct Row;
class Table;

bool evaluateComparison(const ComparisonExpr& expr, const Row& row, const Table& table);
bool evaluateLogical(const LogicalExpr& expr, const Row& row, const Table& table);

} // namespace parallaxdb 