#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <vector>
#include <utility>
#include <functional>
#include <stdexcept>
#include "iterator.hpp"
#include "hash_node.hpp"

namespace shramko
{
  template< class Key, class T, class Hash = std::hash< Key >, class Eq = std::equal_to< Key > >
  class HashTable
  {
  public:
    using cIterator = HashConstIterator< Key, T, Hash, Eq >;
    using iterator = HashIterator< Key, T, Hash, Eq >;

    ~HashTable() noexcept;
    HashTable();
    HashTable(size_t capacity);
    HashTable(const HashTable& rhs);
    HashTable(HashTable&& rhs) noexcept;
    template< class InputIt >
    HashTable(InputIt firstIt, InputIt lastIt);
    HashTable(std::initializer_list< std::pair< Key, T > > init);
    HashTable& operator=(const HashTable& rhs);
    HashTable& operator=(HashTable&& rhs) noexcept;

    iterator begin() noexcept;
    cIterator cbegin() const noexcept;
    iterator end() noexcept;
    cIterator cend() const noexcept;
    T& at(const Key& key);
    const T& at(const Key& key) const;
    T& operator[](const Key& key);
    T& operator[](Key&& key);
    iterator find(const Key& key) noexcept;
    cIterator find(const Key& key) const noexcept;
    bool empty() const noexcept;
    size_t size() const noexcept;
    float loadFactor() const noexcept;
    void rehash(size_t newCapacity);
    float max_load_factor() const noexcept;
    void max_load_factor(float ml);
    std::pair< iterator, bool > insert(const Key& key, const T& value);
    template< class InputIt >
    void insert(InputIt first, InputIt last);
    iterator erase(iterator pos);
    iterator erase(cIterator pos);
    template< class InputIt >
    iterator erase(InputIt first, InputIt last);
    size_t erase(const Key& key);
    void swap(HashTable& rhs) noexcept;
    void clear() noexcept;

  private:
    std::vector< HashNode< Key, T > > slots_;
    size_t capacity_;
    size_t size_;
    float max_load_factor_;

    size_t find_position(const Key& key) const noexcept;
    size_t get_insert_position(const Key& key) const noexcept;
  };

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >::~HashTable() noexcept
  {
    clear();
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >::HashTable() : capacity_(16), size_(0), max_load_factor_(0.75f)
  {
    slots_.resize(capacity_);
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >::HashTable(size_t capacity) : capacity_(capacity), size_(0), max_load_factor_(0.75f)
  {
    slots_.resize(capacity_);
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >::HashTable(const HashTable& rhs):
    capacity_(rhs.capacity_),
    size_(0),
    max_load_factor_(rhs.max_load_factor_)
  {
    slots_.resize(capacity_);
    insert(rhs.begin(), rhs.end());
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >::HashTable(HashTable&& rhs) noexcept:
    slots_(std::move(rhs.slots_)),
    capacity_(rhs.capacity_),
    size_(rhs.size_),
    max_load_factor_(rhs.max_load_factor_)
  {
    rhs.capacity_ = 0;
    rhs.size_ = 0;
  }

  template< class Key, class T, class Hash, class Eq >
  template< class InputIt >
  HashTable< Key, T, Hash, Eq >::HashTable(InputIt firstIt, InputIt lastIt) : capacity_(16), size_(0), max_load_factor_(0.75f)
  {
    slots_.resize(capacity_);
    insert(firstIt, lastIt);
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >::HashTable(std::initializer_list< std::pair< Key, T > > init):
    capacity_(16),
    size_(0),
    max_load_factor_(0.75f)
  {
    slots_.resize(capacity_);
    insert(init.begin(), init.end());
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >& HashTable< Key, T, Hash, Eq >::operator=(const HashTable& rhs)
  {
    if (this != &rhs)
    {
      clear();
      capacity_ = rhs.capacity_;
      max_load_factor_ = rhs.max_load_factor_;
      slots_.resize(capacity_);
      insert(rhs.begin(), rhs.end());
    }
    return *this;
  }

  template< class Key, class T, class Hash, class Eq >
  HashTable< Key, T, Hash, Eq >& HashTable< Key, T, Hash, Eq >::operator=(HashTable&& rhs) noexcept
  {
    if (this != &rhs)
    {
      clear();
      slots_ = std::move(rhs.slots_);
      capacity_ = rhs.capacity_;
      size_ = rhs.size_;
      max_load_factor_ = rhs.max_load_factor_;
      rhs.capacity_ = 0;
      rhs.size_ = 0;
    }
    return *this;
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::iterator HashTable< Key, T, Hash, Eq >::begin() noexcept
  {
    return iterator(slots_.data(), capacity_, 0);
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::cIterator HashTable< Key, T, Hash, Eq >::cbegin() const noexcept
  {
    return cIterator(slots_.data(), capacity_, 0);
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::iterator HashTable< Key, T, Hash, Eq >::end() noexcept
  {
    return iterator(slots_.data(), capacity_, capacity_);
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::cIterator HashTable< Key, T, Hash, Eq >::cend() const noexcept
  {
    return cIterator(slots_.data(), capacity_, capacity_);
  }

  template< class Key, class T, class Hash, class Eq >
  T& HashTable< Key, T, Hash, Eq >::at(const Key& key)
  {
    size_t pos = find_position(key);
    if (pos != capacity_)
    {
      return slots_[pos].data.second;
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class T, class Hash, class Eq >
  const T& HashTable< Key, T, Hash, Eq >::at(const Key& key) const
  {
    size_t pos = find_position(key);
    if (pos != capacity_)
    {
      return slots_[pos].data.second;
    }
    throw std::out_of_range("Key not found");
  }

  template< class Key, class T, class Hash, class Eq >
  T& HashTable< Key, T, Hash, Eq >::operator[](const Key& key)
  {
    size_t pos = get_insert_position(key);
    if (slots_[pos].occupied)
    {
      return slots_[pos].data.second;
    }
    slots_[pos].data = {key, T{}};
    slots_[pos].occupied = true;
    ++size_;
    if (loadFactor() > max_load_factor_)
    {
      rehash(capacity_ * 2);
    }
    return slots_[pos].data.second;
  }

  template< class Key, class T, class Hash, class Eq >
  T& HashTable< Key, T, Hash, Eq >::operator[](Key&& key)
  {
    size_t pos = get_insert_position(key);
    if (slots_[pos].occupied)
    {
      return slots_[pos].data.second;
    }
    slots_[pos].data = {std::move(key), T{}};
    slots_[pos].occupied = true;
    ++size_;
    if (loadFactor() > max_load_factor_)
    {
      rehash(capacity_ * 2);
    }
    return slots_[pos].data.second;
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::iterator HashTable< Key, T, Hash, Eq >::find(const Key& key) noexcept
  {
    size_t pos = find_position(key);
    if (pos != capacity_)
    {
      return iterator(slots_.data(), capacity_, pos);
    }
    return end();
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::cIterator HashTable< Key, T, Hash, Eq >::find(const Key& key) const noexcept
  {
    size_t pos = find_position(key);
    if (pos != capacity_)
    {
      return cIterator(slots_.data(), capacity_, pos);
    }
    return cend();
  }

  template< class Key, class T, class Hash, class Eq >
  bool HashTable< Key, T, Hash, Eq >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< class Key, class T, class Hash, class Eq >
  size_t HashTable< Key, T, Hash, Eq >::size() const noexcept
  {
    return size_;
  }

  template< class Key, class T, class Hash, class Eq >
  float HashTable< Key, T, Hash, Eq >::loadFactor() const noexcept
  {
    return static_cast<float>(size_) / capacity_;
  }

  template< class Key, class T, class Hash, class Eq >
  void HashTable< Key, T, Hash, Eq >::rehash(size_t newCapacity)
  {
    if (newCapacity == 0)
    {
      newCapacity = 1;
    }
    std::vector< HashNode< Key, T > > newSlots(newCapacity);
    std::swap(slots_, newSlots);
    capacity_ = newCapacity;
    size_t oldSize = size_;
    size_ = 0;
    for (size_t i = 0; i < oldCapacity; ++i)
    {
      if (oldSlots[i].occupied && !oldSlots[i].deleted)
      {
        operator[](oldSlots[i].data.first) = oldSlots[i].data.second;
      }
    }
  }

  template< class Key, class T, class Hash, class Eq >
  float HashTable< Key, T, Hash, Eq >::max_load_factor() const noexcept
  {
    return max_load_factor_;
  }

  template< class Key, class T, class Hash, class Eq >
  void HashTable< Key, T, Hash, Eq >::max_load_factor(float ml)
  {
    if (ml <= 0.0f || ml > 1.0f)
    {
      throw std::invalid_argument("Max load factor must be > 0 and <= 1");
    }
    max_load_factor_ = ml;
    if (loadFactor() > ml)
    {
      rehash(capacity_ * 2);
    }
  }

  template< class Key, class T, class Hash, class Eq >
  std::pair< typename HashTable< Key, T, Hash, Eq >::iterator, bool > HashTable< Key, T, Hash, Eq >::insert(const Key& key, const T& value)
  {
    size_t pos = get_insert_position(key);
    if (slots_[pos].occupied)
    {
      return {iterator(slots_.data(), capacity_, pos), false};
    }
    slots_[pos].data = {key, value};
    slots_[pos].occupied = true;
    ++size_;
    if (loadFactor() > max_load_factor_)
    {
      rehash(capacity_ * 2);
    }
    return {iterator(slots_.data(), capacity_, pos), true};
  }

  template< class Key, class T, class Hash, class Eq >
  template< class InputIt >
  void HashTable< Key, T, Hash, Eq >::insert(InputIt first, InputIt last)
  {
    for (; first != last; ++first)
    {
      insert(first->first, first->second);
    }
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::iterator HashTable< Key, T, Hash, Eq >::erase(iterator pos)
  {
    size_t index = pos.current_;
    if (index < capacity_ && slots_[index].occupied && !slots_[index].deleted)
    {
      slots_[index].occupied = false;
      slots_[index].deleted = true;
      --size_;
    }
    return iterator(slots_.data(), capacity_, index);
  }

  template< class Key, class T, class Hash, class Eq >
  typename HashTable< Key, T, Hash, Eq >::iterator HashTable< Key, T, Hash, Eq >::erase(cIterator pos)
  {
    return erase(iterator(slots_.data(), capacity_, pos.current_));
  }

  template< class Key, class T, class Hash, class Eq >
  template< class InputIt >
  typename HashTable< Key, T, Hash, Eq >::iterator HashTable< Key, T, Hash, Eq >::erase(InputIt first, InputIt last)
  {
    iterator result;
    for (auto it = first; it != last; )
    {
      result = erase(it++);
    }
    return result;
  }

  template< class Key, class T, class Hash, class Eq >
  size_t HashTable< Key, T, Hash, Eq >::erase(const Key& key)
  {
    size_t pos = find_position(key);
    if (pos != capacity_)
    {
      slots_[pos].occupied = false;
      slots_[pos].deleted = true;
      --size_;
      return 1;
    }
    return 0;
  }

  template< class Key, class T, class Hash, class Eq >
  void HashTable< Key, T, Hash, Eq >::swap(HashTable& rhs) noexcept
  {
    std::swap(slots_, rhs.slots_);
    std::swap(capacity_, rhs.capacity_);
    std::swap(size_, rhs.size_);
    std::swap(max_load_factor_, rhs.max_load_factor_);
  }

  template< class Key, class T, class Hash, class Eq >
  void HashTable< Key, T, Hash, Eq >::clear() noexcept
  {
    for (size_t i = 0; i < capacity_; ++i)
    {
      slots_[i].occupied = false;
      slots_[i].deleted = false;
    }
    size_ = 0;
  }

  template< class Key, class T, class Hash, class Eq >
  size_t HashTable< Key, T, Hash, Eq >::find_position(const Key& key) const noexcept
  {
    size_t pos = Hash{}(key) % capacity_;
    while (true)
    {
      if (slots_[pos].occupied && !slots_[pos].deleted && Eq{}(slots_[pos].data.first, key))
      {
        return pos;
      }
      if (!slots_[pos].occupied && !slots_[pos].deleted)
      {
        return capacity_;
      }
      pos = (pos + 1) % capacity_;
    }
  }

  template< class Key, class T, class Hash, class Eq >
  size_t HashTable< Key, T, Hash, Eq >::get_insert_position(const Key& key) const noexcept
  {
    size_t pos = Hash{}(key) % capacity_;
    while (slots_[pos].occupied)
    {
      if (!slots_[pos].deleted && Eq{}(slots_[pos].data.first, key))
      {
        return pos;
      }
      pos = (pos + 1) % capacity_;
    }
    return pos;
  }
}

#endif
