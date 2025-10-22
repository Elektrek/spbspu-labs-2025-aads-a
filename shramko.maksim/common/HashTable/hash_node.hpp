#ifndef HASH_NODE_HPP
#define HASH_NODE_HPP

#include <utility>

namespace shramko
{
  template< class Key, class T >
  struct Node
  {
    std::pair< Key, T > data;
    bool occupied = false;
    bool deleted = false;
  };
}

#endif
