#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <iterator>

namespace shramko
{
  template< class Key, class T, class Hash, class Eq >
  class HashIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<const Key, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    HashIterator(HashNode< Key, T >* slots, size_t capacity, size_t current) : slots_(slots), capacity_(capacity), current_(current) {}

    reference operator*() const { return slots_[current_].data; }
    pointer operator->() const { return &slots_[current_].data; }
    HashIterator& operator++();
    bool operator==(const HashIterator& other) const { return current_ == other.current_; }
    bool operator!=(const HashIterator& other) const { return !(*this == other); }

  private:
    HashNode< Key, T >* slots_;
    size_t capacity_;
    size_t current_;
  };

  template< class Key, class T, class Hash, class Eq >
  class HashConstIterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<const Key, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    HashConstIterator(const HashNode< Key, T >* slots, size_t capacity, size_t current) : slots_(slots), capacity_(capacity), current_(current) {}

    reference operator*() const { return slots_[current_].data; }
    pointer operator->() const { return &slots_[current_].data; }
    HashConstIterator& operator++();
    bool operator==(const HashConstIterator& other) const { return current_ == other.current_; }
    bool operator!=(const HashConstIterator& other) const { return !(*this == other); }

  private:
    const HashNode< Key, T >* slots_;
    size_t capacity_;
    size_t current_;
  };
}

#endif
