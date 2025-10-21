#ifndef FWD_LIST_NODE_HPP
#define FWD_LIST_NODE_HPP

namespace shramko
{
  template < typename T >
  struct ListNode
  {
    T dataValue;
    ListNode< T >* nextPtr;

    ListNode():
      nextPtr(nullptr)
    {}
    ListNode(const T& value):
      dataValue(value),
      nextPtr(nullptr)
    {}
    ListNode(T&& value):
      dataValue(std::move(value)),
      nextPtr(nullptr)
    {}
  };
}

#endif
