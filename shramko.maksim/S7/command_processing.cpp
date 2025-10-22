#include "command_processing.hpp"
#include <set>
#include <algorithm>

namespace shramko {

void get_vertexes(std::istream& in, std::ostream& out, const map& graphs) {
    std::string graphName;
    in >> graphName;
    auto it = graphs.find(graphName);
    if (it == graphs.end()) {
        out << "No such graph" << std::endl;
        return;
    }
    const auto& graph = it->second;
    std::vector<std::string> vertexes = graph.get_vertexes();
    for (const auto& v : vertexes) {
        out << v << std::endl;
    }
}

void get_outbound(std::istream& in, std::ostream& out, const map& graphs) {
    std::string graphName, vertexName;
    in >> graphName >> vertexName;
    auto it = graphs.find(graphName);
    if (it == graphs.end()) {
        out << "No such graph" << std::endl;
        return;
    }
    const auto& graph = it->second;
    if (!graph.has_vertex(vertexName)) {
        out << "No such vertex" << std::endl;
        return;
    }
    auto outbounds = graph.get_outbound(vertexName);
    for (const auto& ob : outbounds) {
        out << ob.first << ": ";
        for (int w : ob.second) {
            out << w << " ";
        }
        out << std::endl;
    }
}

void get_inbound(std::istream& in, std::ostream& out, const map& graphs) {
    std::string graphName, vertexName;
    in >> graphName >> vertexName;
    auto it = graphs.find(graphName);
    if (it == graphs.end()) {
        out << "No such graph" << std::endl;
        return;
    }
    const auto& graph = it->second;
    if (!graph.has_vertex(vertexName)) {
        out << "No such vertex" << std::endl;
        return;
    }
    auto inbounds = graph.get_inbound(vertexName);
    for (const auto& ib : inbounds) {
        out << ib.first << ": ";
        for (int w : ib.second) {
            out << w << " ";
        }
        out << std::endl;
    }
}

void create_edge(std::istream& in, map& graphs) {
    std::string graphName, v1, v2;
    int weight;
    in >> graphName >> v1 >> v2 >> weight;
    auto it = graphs.find(graphName);
    if (it == graphs.end())
    {
        out << "No such graph" << std::endl;
        return;
    }
    auto& graph = it->second;
    graph.addEdge(v1, v2, weight);
}

void delete_edge(std::istream& in, map& graphs) {
    std::string graphName, v1, v2;
    int weight;
    in >> graphName >> v1 >> v2 >> weight;
    auto it = graphs.find(graphName);
    if (it == graphs.end())
    {
        out << "No such graph" << std::endl;
        return;
    }
    auto& graph = it->second;
    graph.delete_edge(v1, v2, weight);
}

void create_graph(std::istream& in, map& graphs) {
    std::string graphName;
    int numVertices;
    in >> graphName >> numVertices;
    Graph newGraph;
    for (int i = 0; i < numVertices; ++i) {
        std::string vertex;
        in >> vertex;
        newGraph.add_vertex(vertex);
    }
    graphs[graphName] = newGraph;
}

void merge_graph(std::istream& in, map& graphs) {
    std::string graphName1, graphName2, mergedName;
    in >> mergedName >> graphName1 >> graphName2;
    auto it1 = graphs.find(graphName1);
    auto it2 = graphs.find(graphName2);
    if (it1 == graphs.end() || it2 == graphs.end()) {
        std::cout << "No such graph" << std::endl;
        return;
    }
    Graph mergedGraph = it1->second;
    mergedGraph.merge(it2->second);
    graphs[mergedName] = mergedGraph;
}

void extract_graph(std::istream& in, map& graphs) {
    std::string graphName, extractedName;
    int numVertices;
    in >> extractedName >> graphName >> numVertices;
    auto it = graphs.find(graphName);
    if (it == graphs.end()) {
        std::cout << "No such graph" << std::endl;
        return;
    }
    const auto& graph = it->second;
    std::set<std::string> vertexes;
    for (int i = 0; i < numVertices; ++i) {
        std::string vertex;
        in >> vertex;
        if (graph.has_vertex(vertex)) {
            vertexes.insert(vertex);
        }
    }
    Graph extractedGraph;
    extractedGraph.extract(graph, vertexes);
    graphs[extractedName] = extractedGraph;
}

}
