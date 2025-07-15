# Issue: Complete Week 1 - NULL Value Support & Code Organization

## 🎯 Objective
Complete Week 1 of the ParallaxDB roadmap by implementing NULL value support and improving code organization for better maintainability.

## 📋 NULL Value Support Implementation

### Current State
- `Value` type only supports `int`, `double`, and `string`
- No NULL token in tokenizer
- No NULL handling in expression evaluation
- No NULL support in WHERE clauses

### Required Changes

#### 1. Storage Layer (`include/storage/Table.hpp`)
```cpp
// Update Value type to include NULL
using Value = std::variant<int, double, std::string, std::nullptr_t>;
```

#### 2. Tokenizer (`include/parser/Tokenizer.hpp`)
```cpp
enum class TokenType {
    // ... existing tokens ...
    NULL_VALUE,  // Add this
    IS_NULL,     // Add this
    IS_NOT_NULL, // Add this
};
```

#### 3. Parser (`include/parser/SQLParser.hpp`)
- Add NULL literal parsing: `WHERE column IS NULL`
- Add NULL comparison parsing: `WHERE column = NULL`
- Support `IS NOT NULL` syntax

#### 4. Expression Evaluation (`include/parser/Expression.hpp`)
```cpp
// Update comparison logic to handle NULL
if (std::holds_alternative<std::nullptr_t>(rowValue)) {
    // Handle NULL comparisons
    if (op == "IS NULL") return true;
    if (op == "IS NOT NULL") return false;
    return false; // NULL compared to anything else is false
}
```

#### 5. Tests (`tests/test_parser.cpp`)
```cpp
void test_null_support() {
    // Test NULL literals
    auto plan1 = SQLParser::parse("SELECT * FROM users WHERE age IS NULL", users);
    auto plan2 = SQLParser::parse("SELECT * FROM users WHERE name IS NOT NULL", users);
    
    // Test NULL comparisons
    auto plan3 = SQLParser::parse("SELECT * FROM users WHERE age = NULL", users);
    auto plan4 = SQLParser::parse("SELECT * FROM users WHERE age != NULL", users);
}
```

## 🏗️ Code Organization Improvements

### Current Issues
- All parser logic in single `SQLParser.hpp` file (370+ lines)
- Expression evaluation mixed with parsing logic
- No clear separation between tokenizer, parser, and evaluator
- Missing proper folder structure for different components

### Proposed Structure

#### 1. Reorganize Parser Components
```
include/parser/
├── Tokenizer.hpp          # Lexical analysis
├── SQLParser.hpp          # Main parser (simplified)
├── ExpressionParser.hpp   # WHERE clause expression parsing
├── Expression.hpp         # Expression tree definitions
└── ExpressionEvaluator.hpp # Expression evaluation logic
```

#### 2. Create Dedicated Folders
```
include/
├── parser/           # SQL parsing components
├── planner/          # Query planning and optimization
├── executor/         # Query execution engine
├── storage/          # Data storage and management
├── types/            # Common type definitions
└── utils/            # Utility functions and helpers
```

#### 3. Extract Common Types (`include/types/Common.hpp`)
```cpp
#pragma once
#include <variant>
#include <string>

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
```

#### 4. Split SQLParser.hpp
- **SQLParser.hpp**: Main parsing logic (SELECT, FROM clauses)
- **ExpressionParser.hpp**: WHERE clause expression parsing
- **ExpressionEvaluator.hpp**: Expression evaluation logic

#### 5. Create Query Builder Pattern
```cpp
// include/parser/QueryBuilder.hpp
class QueryBuilder {
public:
    QueryBuilder& select(const std::vector<std::string>& columns);
    QueryBuilder& from(const std::string& table);
    QueryBuilder& where(std::unique_ptr<Expression> expr);
    std::unique_ptr<QueryPlanNode> build();
};
```

## 🧪 Testing Strategy

### NULL Value Tests
1. **Basic NULL Support**
   - `WHERE column IS NULL`
   - `WHERE column IS NOT NULL`
   - `WHERE column = NULL`
   - `WHERE column != NULL`

2. **NULL in Complex Expressions**
   - `WHERE (age IS NULL) OR (age > 30)`
   - `WHERE name IS NOT NULL AND age > 25`
   - `WHERE (age IS NULL) AND (name = 'Alice')`

3. **NULL in Data**
   - Insert rows with NULL values
   - Query tables with NULL data
   - Test NULL propagation in expressions

### Code Organization Tests
1. **Compilation Tests**
   - Ensure all components compile after reorganization
   - Test include dependencies
   - Verify no circular dependencies

2. **Integration Tests**
   - Test parser with new structure
   - Verify expression evaluation still works
   - Test query building pattern

## 📊 Success Criteria

### NULL Support
- [ ] Parse `IS NULL` and `IS NOT NULL` syntax
- [ ] Handle NULL comparisons (`= NULL`, `!= NULL`)
- [ ] Support NULL in complex WHERE expressions
- [ ] Insert and query data with NULL values
- [ ] All NULL tests pass

### Code Organization
- [ ] SQLParser.hpp under 200 lines
- [ ] Clear separation of concerns
- [ ] No circular dependencies
- [ ] Proper folder structure
- [ ] All tests still pass after reorganization

## 🚀 Implementation Plan

### Phase 1: NULL Support (2-3 hours)
1. Update `Value` type to include `std::nullptr_t`
2. Add NULL tokens to tokenizer
3. Implement NULL parsing in SQLParser
4. Update expression evaluation for NULL handling
5. Add comprehensive NULL tests

### Phase 2: Code Organization (3-4 hours)
1. Create new folder structure
2. Extract common types to `types/Common.hpp`
3. Split SQLParser into smaller components
4. Create QueryBuilder pattern
5. Update all includes and dependencies
6. Verify all tests pass

### Phase 3: Documentation (1 hour)
1. Update README with new structure
2. Add inline documentation
3. Create component interaction diagrams

## 🎯 Benefits

### NULL Support Benefits
- **SQL Compliance**: Support standard NULL semantics
- **Data Integrity**: Handle missing/unknown data properly
- **Query Flexibility**: Enable NULL-aware queries
- **Real-world Usage**: Support actual database scenarios

### Code Organization Benefits
- **Maintainability**: Smaller, focused files
- **Readability**: Clear separation of concerns
- **Testability**: Easier to unit test components
- **Extensibility**: Easier to add new features
- **Team Development**: Better for multiple developers

## 🔧 Technical Notes

### NULL Semantics
- `NULL = NULL` → `false` (not `true`)
- `NULL != NULL` → `false` (not `true`)
- `NULL IS NULL` → `true`
- `NULL IS NOT NULL` → `false`
- Any comparison with NULL returns `false` (except IS NULL/IS NOT NULL)

### Performance Considerations
- NULL checks add minimal overhead
- Use `std::holds_alternative<std::nullptr_t>()` for type checking
- Consider NULL-aware indexes for future optimization

## 📝 Acceptance Criteria

- [ ] All NULL value tests pass
- [ ] Code organization follows proposed structure
- [ ] No regression in existing functionality
- [ ] All existing tests still pass
- [ ] Documentation updated
- [ ] Code review completed

---

**Priority**: High (Week 1 completion)
**Estimated Time**: 6-8 hours
**Dependencies**: None (can be implemented independently)
**Labels**: `week1`, `null-support`, `code-organization`, `enhancement` 