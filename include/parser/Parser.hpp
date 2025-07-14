#include "SQLParser.hpp"

namespace parallaxdb {

class Parser {
public:
    static std::unique_ptr<QueryPlanNode> parse(const std::string& query, const Table& table) {
        return SQLParser::parse(query, table);
    }
};

} // namespace parallaxdb
