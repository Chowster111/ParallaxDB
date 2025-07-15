#include <iostream>
#include <cassert>
#include <string>
#include "../include/storage/Table.hpp"
#include "../include/parser/Tokenizer.hpp"
#include "../include/parser/SQLParser.hpp"
#include "../include/types/Common.hpp"

using namespace parallaxdb;

void test_tokenizer() {
    std::cout << "Testing tokenizer..." << std::endl;
    
    // Test basic tokens
    Tokenizer tokenizer("SELECT * FROM users");
    auto tokens = tokenizer.tokenize();
    
    assert(tokens.size() == 5); // SELECT, *, FROM, users, END_OF_INPUT
    assert(tokens[0].type == TokenType::SELECT);
    assert(tokens[1].type == TokenType::STAR);
    assert(tokens[2].type == TokenType::FROM);
    assert(tokens[3].type == TokenType::IDENTIFIER);
    assert(tokens[3].value == "users");
    
    // Test WHERE clause
    Tokenizer tokenizer2("SELECT id, name FROM users WHERE age > 30");
    auto tokens2 = tokenizer2.tokenize();
    
    // Debug output
    std::cout << "Tokens for 'SELECT id, name FROM users WHERE age > 30':" << std::endl;
    for (size_t i = 0; i < tokens2.size(); i++) {
        std::cout << "  " << i << ": " << static_cast<int>(tokens2[i].type) << " '" << tokens2[i].value << "'" << std::endl;
    }
    
    assert(tokens2.size() == 11); // SELECT, id, COMMA, name, FROM, users, WHERE, age, >, 30, END_OF_INPUT
    assert(tokens2[0].type == TokenType::SELECT);
    assert(tokens2[1].type == TokenType::IDENTIFIER);
    assert(tokens2[1].value == "id");
    assert(tokens2[2].type == TokenType::COMMA);
    assert(tokens2[3].type == TokenType::IDENTIFIER);
    assert(tokens2[3].value == "name");
    assert(tokens2[4].type == TokenType::FROM);
    assert(tokens2[5].type == TokenType::IDENTIFIER);
    assert(tokens2[5].value == "users");
    assert(tokens2[6].type == TokenType::WHERE);
    assert(tokens2[7].type == TokenType::IDENTIFIER);
    assert(tokens2[7].value == "age");
    assert(tokens2[8].type == TokenType::GREATER_THAN);
    assert(tokens2[9].type == TokenType::NUMBER);
    assert(tokens2[9].value == "30");
    assert(tokens2[10].type == TokenType::END_OF_INPUT);
    
    // Test string literals
    Tokenizer tokenizer3("SELECT * FROM users WHERE name = 'Alice'");
    auto tokens3 = tokenizer3.tokenize();
    std::cout << "Tokens for 'SELECT * FROM users WHERE name = 'Alice'':" << std::endl;
    for (size_t i = 0; i < tokens3.size(); i++) {
        std::cout << "  " << i << ": " << static_cast<int>(tokens3[i].type) << " '" << tokens3[i].value << "'" << std::endl;
    }
    
    assert(tokens3.size() == 9);
    assert(tokens3[7].type == TokenType::STRING_LITERAL);
    assert(tokens3[7].value == "Alice");
    
    // Test new operators
    Tokenizer tokenizer4("SELECT * FROM users WHERE age >= 30 AND age <= 40");
    auto tokens4 = tokenizer4.tokenize();
    
    // Debug output
    std::cout << "Tokens for 'SELECT * FROM users WHERE age >= 30 AND age <= 40':" << std::endl;
    for (size_t i = 0; i < tokens4.size(); i++) {
        std::cout << "  " << i << ": " << static_cast<int>(tokens4[i].type) << " '" << tokens4[i].value << "'" << std::endl;
    }
    
    assert(tokens4.size() == 13); // SELECT, *, FROM, users, WHERE, age, >=, 30, AND, age, <=, 40, END_OF_INPUT
    assert(tokens4[6].type == TokenType::GREATER_EQUAL);
    assert(tokens4[6].value == ">=");
    assert(tokens4[10].type == TokenType::LESS_EQUAL);
    assert(tokens4[10].value == "<=");
    
    std::cout << "✓ Tokenizer tests passed" << std::endl;
}

void test_parser_basic() {
    std::cout << "Testing basic parser..." << std::endl;
    
    Table users("users", {
        {"id", Column::INT},
        {"name", Column::STRING},
        {"age", Column::INT}
    });
    
    users.insertRow({{1, "Alice", 30}});
    users.insertRow({{2, "Bob", 25}});
    users.insertRow({{3, "Charlie", 35}});
    
    // Test SELECT *
    auto plan1 = SQLParser::parse("SELECT * FROM users", users);
    assert(plan1 != nullptr);
    
    // Test SELECT specific columns
    auto plan2 = SQLParser::parse("SELECT id, name FROM users", users);
    assert(plan2 != nullptr);
    
    // Test WHERE clause
    auto plan3 = SQLParser::parse("SELECT * FROM users WHERE age > 30", users);
    assert(plan3 != nullptr);
    
    // Test WHERE with string
    auto plan4 = SQLParser::parse("SELECT * FROM users WHERE name = 'Alice'", users);
    assert(plan4 != nullptr);
    
    std::cout << "✓ Basic parser tests passed" << std::endl;
}

void test_parser_advanced() {
    std::cout << "Testing advanced parser features..." << std::endl;
    
    Table users("users", {
        {"id", Column::INT},
        {"name", Column::STRING},
        {"age", Column::INT}
    });
    
    users.insertRow({{1, "Alice", 30}});
    users.insertRow({{2, "Bob", 25}});
    users.insertRow({{3, "Charlie", 35}});
    users.insertRow({{4, "Diana", 40}});
    
    // Test AND condition
    auto plan1 = SQLParser::parse("SELECT * FROM users WHERE age > 30 AND age < 40", users);
    assert(plan1 != nullptr);
    
    // Test OR condition
    auto plan2 = SQLParser::parse("SELECT * FROM users WHERE age < 30 OR age > 35", users);
    assert(plan2 != nullptr);
    
    // Test multiple conditions
    auto plan3 = SQLParser::parse("SELECT id, name FROM users WHERE age > 25 AND age < 40", users);
    assert(plan3 != nullptr);
    
    // Test new operators
    auto plan4 = SQLParser::parse("SELECT * FROM users WHERE age >= 30", users);
    assert(plan4 != nullptr);
    
    auto plan5 = SQLParser::parse("SELECT * FROM users WHERE age <= 35", users);
    assert(plan5 != nullptr);
    
    auto plan6 = SQLParser::parse("SELECT * FROM users WHERE name != 'Bob'", users);
    assert(plan6 != nullptr);
    
    // Test parentheses and precedence
    auto plan7 = SQLParser::parse("SELECT * FROM users WHERE (age > 30 AND name = 'Charlie') OR (age < 30)", users);
    assert(plan7 != nullptr);
    auto plan8 = SQLParser::parse("SELECT * FROM users WHERE age > 30 AND (name = 'Charlie' OR name = 'Diana')", users);
    assert(plan8 != nullptr);
    auto plan9 = SQLParser::parse("SELECT * FROM users WHERE (age > 30 OR age < 30) AND name = 'Alice'", users);
    assert(plan9 != nullptr);
    
    std::cout << "✓ Advanced parser tests passed" << std::endl;
}

void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;
    
    Table users("users", {
        {"id", Column::INT},
        {"name", Column::STRING},
        {"age", Column::INT}
    });
    
    users.insertRow({{1, "Alice", 30}});
    
    // Test malformed query (should return nullptr)
    auto plan1 = SQLParser::parse("INVALID QUERY", users);
    assert(plan1 == nullptr); // Should return nullptr on parse error
    
    // Test missing FROM
    auto plan2 = SQLParser::parse("SELECT * users", users);
    assert(plan2 == nullptr);
    
    // Test missing WHERE value
    auto plan3 = SQLParser::parse("SELECT * FROM users WHERE age >", users);
    assert(plan3 == nullptr);
    
    std::cout << "✓ Error handling tests passed" << std::endl;
}

int main() {
    std::cout << "Running enhanced parser tests..." << std::endl;
    
    test_tokenizer();
    test_parser_basic();
    test_parser_advanced();
    test_error_handling();
    
    std::cout << "All enhanced parser tests passed!" << std::endl;
    return 0;
} 