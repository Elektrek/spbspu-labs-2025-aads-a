#ifndef FWD_ITERATOR_HPP
#define FWD_ITERATOR_HPP

#include <iterator>

#include "fwd_list_node.hpp"
#include "fwd_const_iterator.hpp"

namespace shramko
{
  template< typename T >
  class ForwardList;

  template< typename T >
  class FwdIterator: public FwdConstIterator< T >
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    using FwdConstIterator< T >::FwdConstIterator;

    FwdIterator& operator++()
    {
      FwdConstIterator< T >::operator++();
      return *this;
    }

    FwdIterator operator++(int)
    {
      FwdIterator temp = *this;
      FwdConstIterator< T >::operator++();
      return temp;
    }

    reference operator*() const
    {
      return const_cast< reference >(FwdConstIterator< T >::operator*());
    }

    pointer operator->() const
    {
      return const_cast< pointer >(FwdConstIterator< T >::operator->());
    }

  private:
    friend class ForwardList< T >;
  };
}

#endif
