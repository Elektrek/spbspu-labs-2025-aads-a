#ifndef FWD_CONST_ITERATOR_HPP
#define FWD_CONST_ITERATOR_HPP

#include <iterator>

#include "fwd_list_node.hpp"

namespace shramko
{
  template < typename T >
  class FwdConstIterator: public std::iterator< std::forward_iterator_tag, T >
  {
  public:
    using value_type = T;
    using pointer = const T*;
    using reference = const T&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    FwdConstIterator():
      current_(nullptr)
    {}

    explicit FwdConstIterator(const ListNode< T >* node):
      current_(node)
    {}

    reference operator*() const
    {
      return current_->dataValue;
    }

    pointer operator->() const
    {
      return &(operator*());
    }

    FwdConstIterator& operator++()
    {
      current_ = current_->nextPtr;
      return *this;
    }

    FwdConstIterator operator++(int)
    {
      FwdConstIterator tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const FwdConstIterator& other) const
    {
      return current_ == other.current_;
    }

    bool operator!=(const FwdConstIterator& other) const
    {
      return !(*this == other);
    }

  private:
    const ListNode< T >* current_;
  };
}

#endif
