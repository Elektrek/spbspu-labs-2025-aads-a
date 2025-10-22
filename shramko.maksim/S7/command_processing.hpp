#ifndef COMMAND_PROCESSING_HPP
#define COMMAND_PROCESSING_HPP

#include <map>
#include <iostream>
#include "graph.hpp"

namespace shramko
{
  using map = std::map< std::string, Graph >;

  void get_graphs(std::ostream& out, const map& graphs);
  void get_vertexes(std::istream& in, std::ostream& out, const map& graphs);
  void get_outbound(std::istream& in, std::ostream& out, const map& graphs);
  void get_inbound(std::istream& in, std::ostream& out, const map& graphs);
  void create_edge(std::istream& in, map& graphs, std::ostream& out);
  void delete_edge(std::istream& in, map& graphs, std::ostream& out);
  void create_graph(std::istream& in, map& graphs, std::ostream& out);
  void merge_graph(std::istream& in, map& graphs, std::ostream& out);
  void extract_graph(std::istream& in, map& graphs, std::ostream& out);
}

#endif
