#pragma once

#include "Tokenizer.hpp"
#include "../planner/FilterNode.hpp"
#include "../storage/Table.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <stdexcept>

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
    std::vector<WhereClause> whereConditions;
};

class SQLParser {
public:
    static std::unique_ptr<QueryPlanNode> parse(const std::string& query, const Table& table) {
        try {
            ParsedQuery parsed = parseQuery(query);
            return buildQueryPlan(parsed, table);
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            // Return a simple scan as fallback
            return std::make_unique<TableScanNode>(table);
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
            throw std::runtime_error("Expected SELECT");
        }
        pos++;
        
        // Parse column list
        result.select = parseSelectClause(tokens, pos);
        
        // Parse FROM clause
        if (pos >= tokens.size() || tokens[pos].type != TokenType::FROM) {
            throw std::runtime_error("Expected FROM");
        }
        pos++;
        
        if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected table name");
        }
        result.tableName = tokens[pos].value;
        pos++;
        
        // Parse WHERE clause (optional)
        if (pos < tokens.size() && tokens[pos].type == TokenType::WHERE) {
            pos++;
            result.whereConditions = parseWhereClause(tokens, pos);
        }
        
        return result;
    }
    
    static SelectClause parseSelectClause(const std::vector<Token>& tokens, size_t& pos) {
        SelectClause select;
        
        if (pos >= tokens.size()) {
            throw std::runtime_error("Expected column list or *");
        }
        
        if (tokens[pos].type == TokenType::STAR) {
            select.selectAll = true;
            pos++;
        } else {
            select.selectAll = false;
            
            while (pos < tokens.size()) {
                if (tokens[pos].type != TokenType::IDENTIFIER) {
                    throw std::runtime_error("Expected column name");
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
                throw std::runtime_error("Expected column name in WHERE clause");
            }
            condition.column = tokens[pos].value;
            pos++;
            
            // Parse operator
            if (pos >= tokens.size()) {
                throw std::runtime_error("Expected operator in WHERE clause");
            }
            
            if (tokens[pos].type == TokenType::GREATER_THAN) {
                condition.operator_ = ">";
            } else if (tokens[pos].type == TokenType::LESS_THAN) {
                condition.operator_ = "<";
            } else if (tokens[pos].type == TokenType::EQUALS) {
                condition.operator_ = "=";
            } else {
                throw std::runtime_error("Expected comparison operator");
            }
            pos++;
            
            // Parse value
            if (pos >= tokens.size()) {
                throw std::runtime_error("Expected value in WHERE clause");
            }
            
            if (tokens[pos].type == TokenType::NUMBER) {
                try {
                    condition.value = std::stoi(tokens[pos].value);
                } catch (...) {
                    try {
                        condition.value = std::stod(tokens[pos].value);
                    } catch (...) {
                        throw std::runtime_error("Invalid number: " + tokens[pos].value);
                    }
                }
            } else if (tokens[pos].type == TokenType::STRING_LITERAL) {
                condition.value = tokens[pos].value;
            } else {
                throw std::runtime_error("Expected value");
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
    
    static std::unique_ptr<QueryPlanNode> buildQueryPlan(const ParsedQuery& parsed, const Table& table) {
        auto scan = std::make_unique<TableScanNode>(table);
        
        // If no WHERE conditions, return scan
        if (parsed.whereConditions.empty()) {
            return scan;
        }
        
        // Build filter predicate from WHERE conditions
        auto predicate = buildFilterPredicate(parsed.whereConditions, table);
        
        return std::make_unique<FilterNode>(std::move(scan), predicate);
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
            }
        } catch (const std::bad_variant_access&) {
            return false;
        }
        
        return false;
    }
};

} // namespace parallaxdb 