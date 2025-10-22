#ifndef FWD_LIST_NODE_HPP
#define FWDL_IST_NODE_HPP

#include <utility>

namespace shramko
{
  template< typename T >
  struct ListNode
  {
    T dataValue;
    ListNode< T >* nextPtr;

    explicit ListNode(const T& val):
      dataValue(val),
      nextPtr(nullptr)
    {}

    explicit ListNode(T&& val):
      dataValue(std::move(val)),
      nextPtr(nullptr)
    {}
  };
}

#endif
