#ifndef GRAPH_HPP
#define GRAPH_HPP

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
    std::set<std::string> vertexes;

public:
    void add_edge(const std::string& from, const std::string& to, int weight) {
        edges[{from, to}].push_back(weight);
        vertexes.insert(from);
        vertexes.insert(to);
    }

    std::vector<int> getEdges(const std::string& from, const std::string& to) const {
        auto it = edges.find({from, to});
        if (it == edges.end()) {
            return {};
        }
        return it->second;
    }

    std::vector<std::string> get_vertexes() const {
        return {vertexes.cbegin(), vertexes.cend()};
    }

    bool has_vertex(const std::string& vertex) const {
        return vertexes.find(vertex) != vertexes.cend();
    }

    std::vector<std::pair<std::string, std::vector<int>>> get_outbound(const std::string& vertex) const {
        std::vector<std::pair<std::string, std::vector<int>>> outbounds;
        for (auto it = edges.begin(); it != edges.end(); ++it) {
            if (it->first.first == vertex) {
                outbounds.emplace_back(it->first.second, it->second);
            }
        }
        std::sort(outbounds.begin(), outbounds.end());
        return outbounds;
    }

    std::vector<std::pair<std::string, std::vector<int>>> get_inbound(const std::string& vertex) const {
        std::vector<std::pair<std::string, std::vector<int>>> inbounds;
        for (auto it = edges.begin(); it != edges.end(); ++it) {
            if (it->first.second == vertex) {
                inbounds.emplace_back(it->first.first, it->second);
            }
        }
        std::sort(inbounds.begin(), inbounds.end());
        return inbounds;
    }

    void delete_edge(const std::string& from, const std::string& to, int weight) {
        auto it = edges.find({from, to});
        if (it != edges.end()) {
            auto& vec = it->second;
            auto pos = std::find(vec.begin(), vec.end(), weight);
            if (pos != vec.end()) {
                vec.erase(pos);
                if (vec.empty()) {
                    edges.erase({from, to});
                }
            }
        }
    }

    void add_vertex(const std::string& vertex) {
        vertexes.insert(vertex);
    }

    void merge(const Graph& other) {
        for (auto it = other.edges.begin(); it != other.edges.end(); ++it) {
            auto& vec = edges[it->first];
            vec.insert(vec.end(), it->second.begin(), it->second.end());
        }
        for (const auto& v : other.vertexes) {
            vertexes.insert(v);
        }
    }

    void extract(const Graph& g, const std::set<std::string>& vertexes) {
        edges.clear();
        this->vertexes.clear();
        for (auto it = g.edges.begin(); it != g.edges.end(); ++it) {
            if (vertexes.count(it->first.first) && vertexes.count(it->first.second)) {
                edges[it->first] = it->second;
                this->vertexes.insert(it->first.first);
                this->vertexes.insert(it->first.second);
            }
        }
    }
};

}

#endif
