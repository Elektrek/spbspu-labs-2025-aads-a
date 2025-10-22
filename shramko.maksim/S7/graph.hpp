#ifndef SHRAMKO_GRAPH_HPP
#define SHRAMKO_GRAPH_HPP

#include <string>
#include <vector>
#include <set>
#include <algorithm>
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

    std::vector<std::string> get_vertexes() const {
        std::set<std::string> vertexSet;
        for (auto& edge : edges) {
            vertexSet.insert(edge.first.first);
            vertexSet.insert(edge.first.second);
        }
        return std::vector<std::string>(vertexSet.begin(), vertexSet.end());
    }

    bool has_vertex(const std::string& vertex) const {
        for (auto& edge : edges) {
            if (edge.first.first == vertex || edge.first.second == vertex) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::pair<std::string, std::vector<int>>> get_outbound(const std::string& vertex) const {
        std::vector<std::pair<std::string, std::vector<int>>> outbounds;
        for (auto& edge : edges) {
            if (edge.first.first == vertex) {
                outbounds.emplace_back(edge.first.second, edge.second);
            }
        }
        return outbounds;
    }

    std::vector<std::pair<std::string, std::vector<int>>> get_inbound(const std::string& vertex) const {
        std::vector<std::pair<std::string, std::vector<int>>> inbounds;
        for (auto& edge : edges) {
            if (edge.first.second == vertex) {
                inbounds.emplace_back(edge.first.first, edge.second);
            }
        }
        return inbounds;
    }

    void delete_edge(const std::string& from, const std::string& to, int weight) {
        EdgeKey key = {from, to};
        auto it = edges.find(key);
        if (it != edges.end()) {
            auto& vec = it->second;
            auto pos = std::find(vec.begin(), vec.end(), weight);
            if (pos != vec.end()) {
                vec.erase(pos);
                if (vec.empty()) {
                    edges.erase(key);
                }
            }
        }
    }

    void add_vertex(const std::string& vertex) {
    }

    void merge(const Graph& other) {
        for (auto& edge : other.edges) {
            auto& vec = edges[edge.first];
            vec.insert(vec.end(), edge.second.begin(), edge.second.end());
        }
    }

    void extract(const Graph& g, const std::set<std::string>& vertexes) {
        edges.clear();
        for (auto& edge : g.edges) {
            if (vertexes.count(edge.first.first) && vertexes.count(edge.first.second)) {
                edges[edge.first] = edge.second;
            }
        }
    }
};

}

#endif
