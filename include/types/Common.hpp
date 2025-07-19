#pragma once
#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <iostream>

namespace parallaxdb {
    using Value = std::variant<int, double, std::string, std::nullptr_t>;
    
    struct Row {
        std::vector<Value> values;
    };
    
    enum class DataType { 
        INT, 
        DOUBLE, 
        STRING,
        BOOLEAN
    };
    
    struct Constraint {
        enum Type {
            NOT_NULL,
            UNIQUE,
            PRIMARY_KEY,
            CHECK
        };
        
        Type type;
        std::string name;
        std::optional<std::string> checkExpression; // For CHECK constraints
        
        Constraint(Type t, const std::string& n) : type(t), name(n) {}
    };
    
    struct Column {
        std::string name;
        DataType type;
        std::vector<Constraint> constraints;
        std::optional<Value> defaultValue;
        
        Column(const std::string& n, DataType t) : name(n), type(t) {}
    };
    
    struct Schema {
        std::string tableName;
        std::vector<Column> columns;
        std::vector<std::string> primaryKeys;
        
        Schema(const std::string& name) : tableName(name) {}
    };
    
    // Data validation utilities
    class DataValidator {
    public:
        static bool validateValue(const Value& value, DataType type);
        static bool validateRow(const Row& row, const Schema& schema);
        static std::string getTypeName(DataType type);
        static DataType parseTypeName(const std::string& typeName);
    };
    
    // Helper function to print variant values
    inline std::ostream& operator<<(std::ostream& os, const Value& val) {
        std::visit([&os](const auto& v) { os << v; }, val);
        return os;
    }
} 