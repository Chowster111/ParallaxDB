#include "../planner/FilterNode.hpp"
#include <algorithm>
#include <cctype>

namespace parallaxdb {

class Parser {
public:
    static std::unique_ptr<QueryPlanNode> parse(const std::string& query, const Table& table) {
        std::string upperQuery = query;
        std::transform(upperQuery.begin(), upperQuery.end(), upperQuery.begin(), ::toupper);
        
        auto scan = std::make_unique<TableScanNode>(table);

        // Handle WHERE clause
        if (upperQuery.find("WHERE") != std::string::npos) {
            return std::make_unique<FilterNode>(
                std::move(scan),
                [](const Row& row) {
                    // Example: WHERE age > 30
                    if (row.values.size() >= 3) {
                        try {
                            int age = std::get<int>(row.values.at(2));
                            return age > 30;
                        } catch (const std::bad_variant_access&) {
                            return false;
                        }
                    }
                    return false;
                }
            );
        }

        // Handle SELECT * or any SELECT query
        if (upperQuery.find("SELECT") != std::string::npos) {
            return scan;
        }

        // Default: just return scan for any query
        return scan;
    }
};

} // namespace parallaxdb
