#include "../planner/FilterNode.hpp"

class Parser {
public:
    static std::unique_ptr<QueryPlanNode> parse(const std::string& query, const Table& table) {
        auto scan = std::make_unique<TableScanNode>(table);

        // Dummy WHERE clause support
        if (query.find("WHERE") != std::string::npos) {
            return std::make_unique<FilterNode>(
                std::move(scan),
                [](const Row& row) {
                    // Example: WHERE age > 30
                    int age = std::get<int>(row.values.at(2));  // assuming age is 3rd column and values is std::vector<std::variant<...>>
                    return age > 30;
                }
            );
        }

        return scan;
    }
};
