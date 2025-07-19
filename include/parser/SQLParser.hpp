#pragma once

#include "Tokenizer.hpp"
#include "Expression.hpp"
#include "../planner/FilterNode.hpp"
#include "../storage/Table.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <stdexcept>
#include <variant>
#include "ExpressionParser.hpp"
#include "Token.hpp"
#include <iostream>

namespace parallaxdb {

struct SelectClause {
    std::vector<std::string> columns;
    bool selectAll;
    
    SelectClause() : selectAll(false) {}
};

struct WhereClause {
    std::string column;
    std::string operator_;
    Value value;
    std::string logicalOp; // "AND", "OR", or empty
    
    WhereClause() : logicalOp("") {}
};

struct ParsedQuery {
    SelectClause select;
    std::string tableName;
    std::vector<WhereClause> whereConditions; // old
    std::unique_ptr<Expression> whereExpr; // new
};

class SQLParser {
public:
    static std::unique_ptr<QueryPlanNode> parse(const std::string& query, const Table& table) {
        try {
            ParsedQuery parsed = parseQuery(query);
            return buildQueryPlan(parsed, table);
        } catch (const std::exception& e) {
            // Enhanced error reporting
            const char* what = e.what();
            std::string msg = what ? what : "Parse error";
            std::cerr << "Parse error: " << msg << std::endl;
            // If the exception message contains a token position, print the query with a caret
            size_t pos = msg.find("[pos=");
            if (pos != std::string::npos) {
                size_t start = pos + 5;
                size_t end = msg.find("]", start);
                if (end != std::string::npos) {
                    int tokenPos = std::stoi(msg.substr(start, end - start));
                    std::cerr << query << std::endl;
                    for (int i = 0; i < tokenPos; ++i) std::cerr << ' ';
                    std::cerr << "^" << std::endl;
                }
            }
            // Do not execute fallback plan
            return nullptr;
        }
    }

private:
    static ParsedQuery parseQuery(const std::string& query) {
        Tokenizer tokenizer(query);
        auto tokens = tokenizer.tokenize();
        
        ParsedQuery result;
        size_t pos = 0;
        
        // Parse SELECT clause
        if (pos >= tokens.size() || tokens[pos].type != TokenType::SELECT) {
            throw std::runtime_error("Expected SELECT [pos=" + std::to_string(tokens[pos].position) + "]");
        }
        pos++;
        
        // Parse column list
        result.select = parseSelectClause(tokens, pos);
        
        // Parse FROM clause
        if (pos >= tokens.size() || tokens[pos].type != TokenType::FROM) {
            throw std::runtime_error("Expected FROM [pos=" + std::to_string(tokens[pos].position) + "]");
        }
        pos++;
        
        if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected table name [pos=" + std::to_string(tokens[pos].position) + "]");
        }
        result.tableName = tokens[pos].value;
        pos++;
        
        // Parse WHERE clause (optional)
        if (pos < tokens.size() && tokens[pos].type == TokenType::WHERE) {
            pos++;
            result.whereExpr = ExpressionParser::parseWhereExpression(tokens, pos);
        }
        
        return result;
    }
    
    static SelectClause parseSelectClause(const std::vector<Token>& tokens, size_t& pos) {
        SelectClause select;
        
        if (pos >= tokens.size()) {
            throw std::runtime_error("Expected column list or * [pos=" + std::to_string(pos) + "]");
        }
        
        if (tokens[pos].type == TokenType::STAR) {
            select.selectAll = true;
            pos++;
        } else {
            select.selectAll = false;
            
            while (pos < tokens.size()) {
                if (tokens[pos].type != TokenType::IDENTIFIER) {
                    throw std::runtime_error("Expected column name [pos=" + std::to_string(tokens[pos].position) + "]");
                }
                
                select.columns.push_back(tokens[pos].value);
                pos++;
                
                if (pos < tokens.size() && tokens[pos].type == TokenType::COMMA) {
                    pos++;
                } else {
                    break;
                }
            }
        }
        
        return select;
    }
    
    static std::vector<WhereClause> parseWhereClause(const std::vector<Token>& tokens, size_t& pos) {
        std::vector<WhereClause> conditions;
        
        while (pos < tokens.size()) {
            WhereClause condition;
            
            // Parse column name
            if (tokens[pos].type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected column name in WHERE clause [pos=" + std::to_string(tokens[pos].position) + "]");
            }
            condition.column = tokens[pos].value;
            pos++;
            
            // Parse operator
            if (pos >= tokens.size()) {
                throw std::runtime_error("Expected operator in WHERE clause [pos=" + std::to_string(pos) + "]");
            }
            
            if (tokens[pos].type == TokenType::GREATER_THAN) {
                condition.operator_ = ">";
            } else if (tokens[pos].type == TokenType::LESS_THAN) {
                condition.operator_ = "<";
            } else if (tokens[pos].type == TokenType::EQUALS) {
                condition.operator_ = "=";
            } else if (tokens[pos].type == TokenType::GREATER_EQUAL) {
                condition.operator_ = ">=";
            } else if (tokens[pos].type == TokenType::LESS_EQUAL) {
                condition.operator_ = "<=";
            } else if (tokens[pos].type == TokenType::NOT_EQUALS) {
                condition.operator_ = "!=";
            } else {
                throw std::runtime_error("Expected comparison operator [pos=" + std::to_string(tokens[pos].position) + "]");
            }
            pos++;
            
            // Parse value
            if (pos >= tokens.size()) {
                throw std::runtime_error("Expected value in WHERE clause [pos=" + std::to_string(pos) + "]");
            }
            
            if (tokens[pos].type == TokenType::NUMBER) {
                try {
                    condition.value = std::stoi(tokens[pos].value);
                } catch (...) {
                    try {
                        condition.value = std::stod(tokens[pos].value);
                    } catch (...) {
                        throw std::runtime_error("Invalid number: " + tokens[pos].value + " [pos=" + std::to_string(tokens[pos].position) + "]");
                    }
                }
            } else if (tokens[pos].type == TokenType::STRING_LITERAL) {
                condition.value = tokens[pos].value;
            } else {
                throw std::runtime_error("Expected value [pos=" + std::to_string(tokens[pos].position) + "]");
            }
            pos++;
            
            conditions.push_back(condition);
            
            // Check for AND/OR
            if (pos < tokens.size() && 
                (tokens[pos].type == TokenType::AND || tokens[pos].type == TokenType::OR)) {
                condition.logicalOp = tokens[pos].value;
                pos++;
            } else {
                break;
            }
        }
        
        return conditions;
    }
    
    static std::unique_ptr<QueryPlanNode> buildQueryPlan(ParsedQuery& parsed, const Table& table) {
        auto scan = std::make_unique<TableScanNode>(table, parsed.select.selectAll ? std::vector<std::string>{} : parsed.select.columns);
        if (!parsed.whereExpr) {
            return scan;
        }
        return std::make_unique<FilterNode>(std::move(scan), std::move(parsed.whereExpr), table);
    }
    
    static std::function<bool(const Row&)> buildFilterPredicate(
        const std::vector<WhereClause>& conditions, const Table& table) {
        
        return [conditions, &table](const Row& row) -> bool {
            bool result = true;
            std::string logicalOp = "";
            
            for (const auto& condition : conditions) {
                bool conditionResult = evaluateCondition(condition, row, table);
                
                if (logicalOp == "AND") {
                    result = result && conditionResult;
                } else if (logicalOp == "OR") {
                    result = result || conditionResult;
                } else {
                    result = conditionResult;
                }
                
                logicalOp = condition.logicalOp;
            }
            
            return result;
        };
    }
    
    static bool evaluateCondition(const WhereClause& condition, const Row& row, const Table& table) {
        // Find column index
        int columnIndex = -1;
        const auto& columns = table.getColumns();
        
        for (size_t i = 0; i < columns.size(); i++) {
            if (columns[i].name == condition.column) {
                columnIndex = i;
                break;
            }
        }
        
        if (columnIndex == -1 || columnIndex >= static_cast<int>(row.values.size())) {
            return false;
        }
        
        const Value& rowValue = row.values[columnIndex];
        
        try {
            if (condition.operator_ == ">") {
                if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(condition.value)) {
                    return std::get<int>(rowValue) > std::get<int>(condition.value);
                } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(condition.value)) {
                    return std::get<double>(rowValue) > std::get<double>(condition.value);
                }
            } else if (condition.operator_ == "<") {
                if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(condition.value)) {
                    return std::get<int>(rowValue) < std::get<int>(condition.value);
                } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(condition.value)) {
                    return std::get<double>(rowValue) < std::get<double>(condition.value);
                }
            } else if (condition.operator_ == "=") {
                return rowValue == condition.value;
            } else if (condition.operator_ == ">=") {
                if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(condition.value)) {
                    return std::get<int>(rowValue) >= std::get<int>(condition.value);
                } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(condition.value)) {
                    return std::get<double>(rowValue) >= std::get<double>(condition.value);
                }
            } else if (condition.operator_ == "<=") {
                if (std::holds_alternative<int>(rowValue) && std::holds_alternative<int>(condition.value)) {
                    return std::get<int>(rowValue) <= std::get<int>(condition.value);
                } else if (std::holds_alternative<double>(rowValue) && std::holds_alternative<double>(condition.value)) {
                    return std::get<double>(rowValue) <= std::get<double>(condition.value);
                }
            } else if (condition.operator_ == "!=") {
                return rowValue != condition.value;
            }
        } catch (const std::bad_variant_access&) {
            return false;
        }
        
        return false;
    }
};

} // namespace parallaxdb 