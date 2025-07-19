#include <iostream>
#include <string>
#include "../include/storage/Database.hpp"
#include "../include/parser/SQLProcessor.hpp"
#include "../include/types/Common.hpp"

using namespace parallaxdb;

int main() {
    std::cout << "Welcome to ParallaxDB!\n";
    std::cout << "Supported commands: SELECT, INSERT, CREATE TABLE, DROP TABLE\n\n";

    Database db;
    
    // Create a sample table for demonstration
    Schema usersSchema("users");
    usersSchema.columns = {
        {"id", DataType::INT},
        {"name", DataType::STRING},
        {"age", DataType::INT}
    };
    
    db.createTable("users", usersSchema);
    
    // Insert sample data
    db.insertInto("users", {1, "Alice", 30});
    db.insertInto("users", {2, "Bob", 25});
    db.insertInto("users", {3, "Charlie", 35});
    db.insertInto("users", {4, "Diana", 40});

    while (true) {
        std::cout << "\n> ";
        std::string query;
        std::getline(std::cin, query);
        
        if (query == "exit" || query == "quit") {
            break;
        }
        
        if (query.empty()) {
            continue;
        }

        try {
            SQLProcessor::processStatement(query, db);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
