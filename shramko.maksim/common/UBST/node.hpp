#ifndef NODE_HPP
#define NODE_HPP

#include <utility>

namespace shramko
{
  template < typename Key, typename Value >
  struct Node
  {
    Key key;
    Value value;
    Node* parent;
    Node* left;
    Node* right;

    Node(const Key& k, const Value& v):
      key(k),
      value(v),
      parent(nullptr),
      left(nullptr),
      right(nullptr)
    {}
  };
}

#endif
