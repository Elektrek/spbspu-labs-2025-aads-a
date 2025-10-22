#ifndef FWD_CONST_ITERATOR_HPP
#define FWD_CONST_ITERATOR_HPP

#include <iterator>

#include "fwd_list_node.hpp"

namespace shramko
{
  template< typename T >
  class ForwardList;

  template< typename T >
  class FwdConstIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    FwdConstIterator():
      node_(nullptr),
      isAtBegin_(true)
    {}

    explicit FwdConstIterator(ListNode< T >* node):
      node_(node),
      isAtBegin_(true)
    {}

    FwdConstIterator& operator++()
    {
      node_ = node_->nextPtr;
      isAtBegin_ = false;
      return *this;
    }

    FwdConstIterator operator++(int)
    {
      FwdConstIterator temp = *this;
      ++(*this);
      return temp;
    }

    reference operator*() const
    {
      return node_->dataValue;
    }

    pointer operator->() const
    {
      return &node_->dataValue;
    }

    bool operator==(const FwdConstIterator& other) const
    {
      return node_ == other.node_ && isAtBegin_ == other.isAtBegin_;
    }

    bool operator!=(const FwdConstIterator& other) const
    {
      return !(*this == other);
    }

  private:
    friend class ForwardList< T >;
    ListNode< T >* node_;
    bool isAtBegin_;
  };
}

#endif
