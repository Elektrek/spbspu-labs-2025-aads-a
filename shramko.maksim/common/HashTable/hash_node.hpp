#ifndef HASH_NODE_HPP
#define HASH_NODE_HPP

#include <utility>

namespace shramko
{
  template < typename Key, typename Value >
  struct HashNode
  {
    Key key;
    Value value;
    bool occupied = false;
    bool deleted = false;
  };
}

#endif
