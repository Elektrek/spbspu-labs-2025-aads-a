#ifndef NODE_HPP
#define NODE_HPP

#include <utility>

namespace shramko
{
  template < class Key, class T >
  struct Node
  {
    Key key;
    T value;
    bool occupied;
    bool deleted;

    Node() : occupied(false), deleted(false) {}
  };
}

#endif
