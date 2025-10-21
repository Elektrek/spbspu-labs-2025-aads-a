#ifndef FWD_ITERATOR_HPP
#define FWD_ITERATOR_HPP

#include <iterator>

#include "fwd_list_node.hpp"

namespace shramko
{
  template < typename T >
  class FwdIterator: public std::iterator< std::forward_iterator_tag, T >
  {
  public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    FwdIterator():
      current_(nullptr)
    {}

    explicit FwdIterator(ListNode< T >* node):
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

    FwdIterator& operator++()
    {
      current_ = current_->nextPtr;
      return *this;
    }

    FwdIterator operator++(int)
    {
      FwdIterator tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const FwdIterator& other) const
    {
      return current_ == other.current_;
    }

    bool operator!=(const FwdIterator& other) const
    {
      return !(*this == other);
    }

  private:
    ListNode< T >* current_;
  };
}

#endif
