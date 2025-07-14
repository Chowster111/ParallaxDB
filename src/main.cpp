#include <iostream>
#include <string>
#include "storage/Table.hpp"
#include "parser/Parser.hpp"

using namespace parallaxdb;

int main() {
    std::cout << "Welcome to ParallaxDB!\n";

    Table users(
        "users",
        {
            {"id", Column::INT},
            {"name", Column::STRING},
            {"age", Column::INT}
        }
    );

    users.insertRow({{1, "Alice", 30}});
    users.insertRow({{2, "Bob", 25}});
    users.insertRow({{3, "Charlie", 35}});
    users.insertRow({{4, "Diana", 40}});

    while (true) {
        std::cout << "\n> ";
        std::string query;
        std::getline(std::cin, query);
        if (query == "exit") break;

        auto plan = Parser::parse(query, users);
        if (plan) {
            plan->execute();
        }
    }

    return 0;
}
