#include <iostream>
#include <cassert>
#include "../include/storage/Database.hpp"
#include "../include/parser/SQLProcessor.hpp"
#include "../include/types/Common.hpp"

using namespace parallaxdb;

void test_basic_table_operations() {
    std::cout << "Testing basic table operations..." << std::endl;
    
    Database db;
    
    Schema usersSchema("users");
    usersSchema.columns = {
        {"id", DataType::INT},
        {"name", DataType::STRING},
        {"age", DataType::INT}
    };
    
    db.createTable("users", usersSchema);
    
    db.insertInto("users", {1, "Alice", 30});
    db.insertInto("users", {2, "Bob", 25});
    
    const Table* users = db.getTable("users");
    assert(users != nullptr);
    assert(users->getRows().size() == 2);
    std::cout << "✓ Basic table operations passed" << std::endl;
}

void test_query_execution() {
    std::cout << "Testing query execution..." << std::endl;
    
    Database db;
    
    Schema usersSchema("users");
    usersSchema.columns = {
        {"id", DataType::INT},
        {"name", DataType::STRING},
        {"age", DataType::INT}
    };
    
    db.createTable("users", usersSchema);
    
    db.insertInto("users", {1, "Alice", 30});
    db.insertInto("users", {2, "Bob", 25});
    db.insertInto("users", {3, "Charlie", 35});
    
    // Test SELECT *
    SQLProcessor::processStatement("SELECT * FROM users", db);
    
    // Test WHERE clause
    SQLProcessor::processStatement("SELECT * FROM users WHERE age > 30", db);
    
    std::cout << "✓ Query execution tests passed" << std::endl;
}

int main() {
    std::cout << "Running ParallaxDB tests..." << std::endl;
    
    test_basic_table_operations();
    test_query_execution();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
} 