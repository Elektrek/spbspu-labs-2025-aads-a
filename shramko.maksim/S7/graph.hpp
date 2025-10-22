#ifndef SHRAMKO_GRAPH_HPP
#define SHRAMKO_GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <HashTable/hash_table.hpp>
#include <functional>

namespace shramko {

struct PairHash {
    std::size_t operator()(const std::pair<std::string, std::string>& p) const {
        return std::hash<std::string>{}(p.first) ^ std::hash<std::string>{}(p.second);
    }
};

class Graph {
private:
    using EdgeKey = std::pair<std::string, std::string>;
    using EdgeValue = std::vector<int>;
    using EdgeEq = std::equal_to<EdgeKey>;

    HashTable<EdgeKey, EdgeValue, PairHash, EdgeEq> edges;

public:
    void addEdge(const std::string& from, const std::string& to, int weight) {
        EdgeKey key = {from, to};
        auto& vec = edges[key];
        vec.push_back(weight);
    }

    std::vector<int> getEdges(const std::string& from, const std::string& to) const {
        EdgeKey key = {from, to};
        auto it = edges.find(key);
        if (it == edges.end()) {
            return {};
        }
        return it->second;
    }

};

}

#endif
