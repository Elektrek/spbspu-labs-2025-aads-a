#ifndef NODE_HPP
#define NODE_HPP

#include <utility>

namespace shramko
{
  template < typename Key, typename Value >
  struct Node
  {
    std::pair< Key, Value > data;
    Node* parent;
    Node* left;
    Node* right;

    Node(const Key& k, const Value& v):
      data(k, v),
      parent(nullptr),
      left(nullptr),
      right(nullptr)
    {}
  };
}

#endif