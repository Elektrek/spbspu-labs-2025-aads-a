#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <cassert>
#include <thread>

#include "node.hpp"

namespace shramko
{
  template < class Key, class T, class Hash, class Eq >
  class HashIterator
  {
  public:
    using value_type = std::pair< const Key, T >;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    HashIterator() : slots_(nullptr), capacity_(0), current_(0) {}

    HashIterator(Node< Key, T >* slots, size_t capacity, size_t current) : slots_(slots), capacity_(capacity), current_(current)
    {
      find_occupied();
    }

    reference operator*()
    {
      static thread_local value_type temp;
      temp = {slots_[current_].key, slots_[current_].value};
      return temp;
    }

    pointer operator->()
    {
      static thread_local value_type temp;
      temp = {slots_[current_].key, slots_[current_].value};
      return &temp;
    }

    HashIterator& operator++()
    {
      ++current_;
      find_occupied();
      return *this;
    }

    HashIterator operator++(int)
    {
      HashIterator tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const HashIterator& rhs) const noexcept
    {
      return current_ == rhs.current_;
    }

    bool operator!=(const HashIterator& rhs) const noexcept
    {
      return !(*this == rhs);
    }

  private:
    Node< Key, T >* slots_;
    size_t capacity_;
    size_t current_;

    void find_occupied()
    {
      while (current_ < capacity_ && (!slots_[current_].occupied || slots_[current_].deleted))
      {
        ++current_;
      }
    }

    friend class HashTable< Key, T, Hash, Eq >;
  };

  template < class Key, class T, class Hash, class Eq >
  class HashConstIterator
  {
  public:
    using value_type = std::pair< const Key, T >;
    using pointer = const value_type*;
    using reference = const value_type&;
    using difference_type = ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    using this_t = HashConstIterator< Key, T, Hash, Eq >;

    HashConstIterator() : slots_(nullptr), capacity_(0), current_(0) {}

    HashConstIterator(const Node< Key, T >* slots, size_t capacity, size_t current) : slots_(slots), capacity_(capacity), current_(current)
    {
      find_occupied();
    }

    reference operator*() const
    {
      static thread_local value_type temp;
      temp = {slots_[current_].key, slots_[current_].value};
      return temp;
    }

    pointer operator->() const
    {
      static thread_local value_type temp;
      temp = {slots_[current_].key, slots_[current_].value};
      return &temp;
    }

    this_t& operator++()
    {
      ++current_;
      find_occupied();
      return *this;
    }

    this_t operator++(int)
    {
      this_t tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const this_t& rhs) const noexcept
    {
      return current_ == rhs.current_;
    }

    bool operator!=(const this_t& rhs) const noexcept
    {
      return !(*this == rhs);
    }

  private:
    const Node< Key, T >* slots_;
    size_t capacity_;
    size_t current_;

    void find_occupied() const
    {
      while (current_ < capacity_ && (!slots_[current_].occupied || slots_[current_].deleted))
      {
        ++current_;
      }
    }

    friend class HashTable< Key, T, Hash, Eq >;
  };
}

#endif
