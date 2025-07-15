#include "../../include/parser/ExpressionEvaluator.hpp"
#include "../../include/parser/Expression.hpp"

namespace parallaxdb {

bool evaluateComparison(const ComparisonExpr& expr, const Row& row, const Table& table) {
    int columnIndex = -1;
    const auto& columns = table.getColumns();
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i].name == expr.column) {
            columnIndex = i;
            break;
        }
    }
    if (columnIndex == -1 || columnIndex >= static_cast<int>(row.values.size())) {
        return false;
    }
    const Value& rowValue = row.values[columnIndex];
    try {
        if (expr.op == ">") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(expr.value)) {
                return std::get<int>(rowValue) > std::get<int>(expr.value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(expr.value)) {
                return std::get<double>(rowValue) > std::get<double>(expr.value);
            }
        } else if (expr.op == "<") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(expr.value)) {
                return std::get<int>(rowValue) < std::get<int>(expr.value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(expr.value)) {
                return std::get<double>(rowValue) < std::get<double>(expr.value);
            }
        } else if (expr.op == "=") {
            return rowValue == expr.value;
        } else if (expr.op == ">=") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(expr.value)) {
                return std::get<int>(rowValue) >= std::get<int>(expr.value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(expr.value)) {
                return std::get<double>(rowValue) >= std::get<double>(expr.value);
            }
        } else if (expr.op == "<=") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(expr.value)) {
                return std::get<int>(rowValue) <= std::get<int>(expr.value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(expr.value)) {
                return std::get<double>(rowValue) <= std::get<double>(expr.value);
            }
        } else if (expr.op == "!=") {
            return rowValue != expr.value;
        }
    } catch (const std::bad_variant_access&) {
        return false;
    }
    return false;
}

bool evaluateLogical(const LogicalExpr& expr, const Row& row, const Table& table) {
    if (expr.op == "AND") {
        return expr.left->evaluate(row, table) && expr.right->evaluate(row, table);
    } else if (expr.op == "OR") {
        return expr.left->evaluate(row, table) || expr.right->evaluate(row, table);
    }
    return false;
}

// Out-of-line method definitions
bool ComparisonExpr::evaluate(const Row& row, const Table& table) const {
    return evaluateComparison(*this, row, table);
}

bool LogicalExpr::evaluate(const Row& row, const Table& table) const {
    return evaluateLogical(*this, row, table);
}

} // namespace parallaxdb 