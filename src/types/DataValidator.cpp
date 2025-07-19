#include "../../include/types/Common.hpp"
#include <stdexcept>
#include <algorithm>

namespace parallaxdb {

bool DataValidator::validateValue(const Value& value, DataType type) {
    switch (type) {
        case DataType::INT:
            return std::holds_alternative<int>(value);
        case DataType::DOUBLE:
            return std::holds_alternative<double>(value) || std::holds_alternative<int>(value);
        case DataType::STRING:
            return std::holds_alternative<std::string>(value);
        case DataType::BOOLEAN:
            return std::holds_alternative<int>(value) && 
                   (std::get<int>(value) == 0 || std::get<int>(value) == 1);
        default:
            return false;
    }
}

bool DataValidator::validateRow(const Row& row, const Schema& schema) {
    if (row.values.size() != schema.columns.size()) {
        return false;
    }
    
    for (size_t i = 0; i < row.values.size(); ++i) {
        if (!validateValue(row.values[i], schema.columns[i].type)) {
            return false;
        }
        
        // Check NOT NULL constraint
        for (const auto& constraint : schema.columns[i].constraints) {
            if (constraint.type == Constraint::NOT_NULL) {
                if (std::holds_alternative<std::nullptr_t>(row.values[i])) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::string DataValidator::getTypeName(DataType type) {
    switch (type) {
        case DataType::INT: return "INT";
        case DataType::DOUBLE: return "DOUBLE";
        case DataType::STRING: return "STRING";
        case DataType::BOOLEAN: return "BOOLEAN";
        default: return "UNKNOWN";
    }
}

DataType DataValidator::parseTypeName(const std::string& typeName) {
    std::string upper = typeName;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "INT" || upper == "INTEGER") return DataType::INT;
    if (upper == "DOUBLE" || upper == "FLOAT" || upper == "REAL") return DataType::DOUBLE;
    if (upper == "STRING" || upper == "VARCHAR" || upper == "TEXT") return DataType::STRING;
    if (upper == "BOOLEAN" || upper == "BOOL") return DataType::BOOLEAN;
    
    throw std::runtime_error("Unknown data type: " + typeName);
}

} // namespace parallaxdb 