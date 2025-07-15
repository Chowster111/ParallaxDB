#include <iostream>
#include <cassert>
#include "../include/storage/Table.hpp"
#include "../include/parser/Parser.hpp"
#include "../include/types/Common.hpp"

using namespace parallaxdb;

void test_basic_table_operations() {
    std::cout << "Testing basic table operations..." << std::endl;
    
    Table users("users", {
        {"id", Column::INT},
        {"name", Column::STRING},
        {"age", Column::INT}
    });
    
    users.insertRow({{1, "Alice", 30}});
    users.insertRow({{2, "Bob", 25}});
    
    assert(users.getRows().size() == 2);
    std::cout << "✓ Basic table operations passed" << std::endl;
}

void test_query_execution() {
    std::cout << "Testing query execution..." << std::endl;
    
    Table users("users", {
        {"id", Column::INT},
        {"name", Column::STRING},
        {"age", Column::INT}
    });
    
    users.insertRow({{1, "Alice", 30}});
    users.insertRow({{2, "Bob", 25}});
    users.insertRow({{3, "Charlie", 35}});
    
    // Test SELECT *
    auto plan1 = Parser::parse("SELECT * FROM users", users);
    assert(plan1 != nullptr);
    
    // Test WHERE clause
    auto plan2 = Parser::parse("SELECT * FROM users WHERE age > 30", users);
    assert(plan2 != nullptr);
    
    std::cout << "✓ Query execution tests passed" << std::endl;
}

int main() {
    std::cout << "Running ParallaxDB tests..." << std::endl;
    
    test_basic_table_operations();
    test_query_execution();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
} 