#pragma once
#include <variant>
#include <string>
#include <vector>

namespace parallaxdb {
    using Value = std::variant<int, double, std::string, std::nullptr_t>;
    
    struct Row {
        std::vector<Value> values;
    };
    
    struct Column {
        std::string name;
        enum Type { INT, DOUBLE, STRING } type;
    };
} 