#ifndef CONST_ITERATOR_HPP
#define CONST_ITERATOR_HPP

#include <iterator>
#include "node.hpp"

namespace shramko
{
  template < class Key, class Value, class Compare >
  class ConstIterator: public std::iterator< std::bidirectional_iterator_tag, std::pair< const Key, Value > >
  {
  public:
    using value_type = std::pair< const Key, Value >;
    using pointer = value_type*;
    using reference = value_type;
    using difference_type = ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    ConstIterator():
      node_(nullptr)
    {}

    explicit ConstIterator(const Node< Key, Value >* node):
      node_(node)
    {}

    const value_type& operator*() const
    {
      static thread_local value_type temp;
      temp = {node_->key, node_->value};
      return temp;
    }

    const value_type* operator->() const
    {
      static thread_local value_type temp;
      temp = {node_->key, node_->value};
      return &temp;
    }

    ConstIterator& operator++()
    {
      if (node_->right)
      {
        node_ = node_->right;
        while (node_->left)
        {
          node_ = node_->left;
        }
      }
      else
      {
        while (node_->parent && node_ == node_->parent->right)
        {
          node_ = node_->parent;
        }
        node_ = node_->parent;
      }
      return *this;
    }

    ConstIterator operator++(int)
    {
      ConstIterator tmp = *this;
      ++*this;
      return tmp;
    }

    ConstIterator& operator--()
    {
      if (node_->left)
      {
        node_ = node_->left;
        while (node_->right)
        {
          node_ = node_->right;
        }
      }
      else
      {
        while (node_->parent && node_ == node_->parent->left)
        {
          node_ = node_->parent;
        }
        node_ = node_->parent;
      }
      return *this;
    }

    ConstIterator operator--(int)
    {
      ConstIterator tmp = *this;
      --*this;
      return tmp;
    }

    bool operator==(const ConstIterator& other) const
    {
      return node_ == other.node_;
    }

    bool operator!=(const ConstIterator& other) const
    {
      return !(*this == other);
    }

  private:
    const Node< Key, Value >* node_;
  };
}

#endif
