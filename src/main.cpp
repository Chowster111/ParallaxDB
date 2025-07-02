#include <iostream>
#include "storage/Table.hpp"
#include "parser/Parser.hpp"

using namespace parallaxdb;

int main() {
    std::cout << "Welcome to ParallaxDB!" << std::endl;

    // Step 1: Define table schema
    Table users(
        "users",
        {
            {"id", Column::INT},
            {"name", Column::STRING},
            {"age", Column::INT}
        }
    );

    // Step 2: Insert sample data
    users.insertRow({{1, "Alice", 30}});
    users.insertRow({{2, "Bob", 25}});
    users.insertRow({{3, "Charlie", 35}});

    // Step 3: Parse query (stub)
    std::string query = "SELECT * FROM users;";
    auto plan = Parser::parse(query, users);

    // Step 4: Execute plan
    plan->execute();

    return 0;
}
