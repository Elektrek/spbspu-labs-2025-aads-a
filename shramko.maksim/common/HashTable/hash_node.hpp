#ifndef HASH_NODE_HPP
#define HASH_NODE_HPP

#include <utility>

namespace shramko
{
  template < typename Key, typename Value >
  struct Node
  {
    std::pair< const Key, Value > data;
    bool occupied = false;
    bool deleted = false;
  };
}

#endif
