#pragma once
#include <string>
#include <memory>
#include "../types/Common.hpp"
#include "../storage/Table.hpp"
#include "ExpressionEvaluator.hpp"

namespace parallaxdb {

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

} // namespace parallaxdb 