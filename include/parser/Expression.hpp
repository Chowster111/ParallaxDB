#pragma once
#include <string>
#include <memory>
#include <variant>
#include "../storage/Table.hpp"

namespace parallaxdb {

using Value = std::variant<int, double, std::string>;
struct Row;

struct Expression {
    virtual ~Expression() = default;
    virtual bool evaluate(const Row& row, const Table& table) const = 0;
};

struct ComparisonExpr : public Expression {
    std::string column;
    std::string op;
    Value value;
    ComparisonExpr(const std::string& c, const std::string& o, const Value& v)
        : column(c), op(o), value(v) {}
    bool evaluate(const Row& row, const Table& table) const override;
};

struct LogicalExpr : public Expression {
    std::string op; // "AND" or "OR"
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    LogicalExpr(const std::string& o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
    bool evaluate(const Row& row, const Table& table) const override;
};

struct ParenExpr : public Expression {
    std::unique_ptr<Expression> expr;
    ParenExpr(std::unique_ptr<Expression> e) : expr(std::move(e)) {}
    bool evaluate(const Row& row, const Table& table) const override {
        return expr->evaluate(row, table);
    }
};

inline bool ComparisonExpr::evaluate(const Row& row, const Table& table) const {
    int columnIndex = -1;
    const auto& columns = table.getColumns();
    for (size_t i = 0; i < columns.size(); i++) {
        if (columns[i].name == column) {
            columnIndex = i;
            break;
        }
    }
    if (columnIndex == -1 || columnIndex >= static_cast<int>(row.values.size())) {
        return false;
    }
    const Value& rowValue = row.values[columnIndex];
    try {
        if (op == ">") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(value)) {
                return std::get<int>(rowValue) > std::get<int>(value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(value)) {
                return std::get<double>(rowValue) > std::get<double>(value);
            }
        } else if (op == "<") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(value)) {
                return std::get<int>(rowValue) < std::get<int>(value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(value)) {
                return std::get<double>(rowValue) < std::get<double>(value);
            }
        } else if (op == "=") {
            return rowValue == value;
        } else if (op == ">=") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(value)) {
                return std::get<int>(rowValue) >= std::get<int>(value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(value)) {
                return std::get<double>(rowValue) >= std::get<double>(value);
            }
        } else if (op == "<=") {
            if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(value)) {
                return std::get<int>(rowValue) <= std::get<int>(value);
            } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(value)) {
                return std::get<double>(rowValue) <= std::get<double>(value);
            }
        } else if (op == "!=") {
            return rowValue != value;
        }
    } catch (const std::bad_variant_access&) {
        return false;
    }
    return false;
}

inline bool LogicalExpr::evaluate(const Row& row, const Table& table) const {
    if (op == "AND") {
        return left->evaluate(row, table) && right->evaluate(row, table);
    } else if (op == "OR") {
        return left->evaluate(row, table) || right->evaluate(row, table);
    }
    return false;
}

} // namespace parallaxdb 