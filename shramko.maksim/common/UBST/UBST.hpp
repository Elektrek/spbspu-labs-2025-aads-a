#ifndef UBST_HPP
#define UBST_HPP

#include <cstddef>
#include <stdexcept>
#include <stack>
#include <queue>
#include <new>
#include "node.hpp"
#include "constiterator.hpp"

namespace shramko
{
  template < typename Key, typename Value, typename Compare = std::less< Key > >
  class UBstTree
  {
  public:
    using value_type = std::pair<const Key, Value>;
    using const_iterator = ConstIterator< Key, Value, Compare >;
    using const_reverse_iterator = std::reverse_iterator< const_iterator >;

    UBstTree();
    UBstTree(const UBstTree& other);
    UBstTree(UBstTree&& other) noexcept;
    ~UBstTree();
    UBstTree& operator=(const UBstTree& other);
    UBstTree& operator=(UBstTree&& other) noexcept;

    bool empty() const noexcept;
    size_t size() const noexcept;
    void clear() noexcept;
    void swap(UBstTree& other) noexcept;
    Value& operator[](const Key& key);
    const Value& operator[](const Key& key) const;
    Value& at(const Key& key);
    const Value& at(const Key& key) const;

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    const_iterator find(const Key& key) const noexcept;

    size_t erase(const Key& key);

    template < typename F >
    F traverse_lnr(F f) const;

    template < typename F >
    F traverse_rnl(F f) const;

    template < typename F >
    F traverse_breadth(F f) const;

    friend class ConstIterator< Key, Value, Compare >;

  private:
    Node< Key, Value >* root_;
    size_t size_;
    Compare comp_;

    void clearNode(Node< Key, Value >* node);

    Node< Key, Value >* insertNode(Node< Key, Value >* node, const Key& key,
                                   const Value& value, Node< Key, Value >* parent, size_t& size);

    Node< Key, Value >* findNode(Node< Key, Value >* node, const Key& key);
    const Node< Key, Value >* findNode(const Node< Key, Value >* node, const Key& key) const;

    const Node< Key, Value >* minNode(const Node< Key, Value >* node) const;
    const Node< Key, Value >* maxNode(const Node< Key, Value >* node) const;

    void copyTree(Node< Key, Value >*& node, Node< Key, Value >* otherNode, Node< Key, Value >* parent);

    Node< Key, Value >* eraseNode(Node< Key, Value >* node, const Key& key, size_t& size);
  };

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::UBstTree():
    root_(nullptr),
    size_(0),
    comp_(Compare())
  {}

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::UBstTree(const UBstTree& other):
    root_(nullptr),
    size_(0),
    comp_(other.comp_)
  {
    try
    {
      copyTree(root_, other.root_, nullptr);
    }
    catch (std::bad_alloc&)
    {
      clear();
      throw;
    }
  }

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::UBstTree(UBstTree&& other) noexcept:
    root_(other.root_),
    size_(other.size_),
    comp_(std::move(other.comp_))
  {
    other.root_ = nullptr;
    other.size_ = 0;
  }

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::~UBstTree()
  {
    clear();
  }

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >& UBstTree< Key, Value, Compare >::operator=(const UBstTree& other)
  {
    if (this == &other)
    {
      return *this;
    }
    UBstTree temp(other);
    swap(temp);
    return *this;
  }

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >& UBstTree< Key, Value, Compare >::operator=(UBstTree&& other) noexcept
  {
    if (this == &other)
    {
      return *this;
    }
    clear();
    root_ = other.root_;
    size_ = other.size_;
    comp_ = std::move(other.comp_);
    other.root_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  template < typename Key, typename Value, typename Compare >
  bool UBstTree< Key, Value, Compare >::empty() const noexcept
  {
    return size_ == 0;
  }

  template < typename Key, typename Value, typename Compare >
  size_t UBstTree< Key, Value, Compare >::size() const noexcept
  {
    return size_;
  }

  template < typename Key, typename Value, typename Compare >
  void UBstTree< Key, Value, Compare >::clear() noexcept
  {
    clearNode(root_);
    root_ = nullptr;
    size_ = 0;
  }

  template < typename Key, typename Value, typename Compare >
  void UBstTree< Key, Value, Compare >::swap(UBstTree& other) noexcept
  {
    std::swap(root_, other.root_);
    std::swap(size_, other.size_);
    std::swap(comp_, other.comp_);
  }

  template < typename Key, typename Value, typename Compare >
  Value& UBstTree< Key, Value, Compare >::operator[](const Key& key)
  {
    Node< Key, Value >* node = findNode(root_, key);
    if (node)
    {
      return node->value;
    }
    root_ = insertNode(root_, key, Value{}, nullptr, size_);
    node = findNode(root_, key);
    return node->value;
  }

  template < typename Key, typename Value, typename Compare >
  const Value& UBstTree< Key, Value, Compare >::operator[](const Key& key) const
  {
    return at(key);
  }

  template < typename Key, typename Value, typename Compare >
  Value& UBstTree< Key, Value, Compare >::at(const Key& key)
  {
    Node< Key, Value >* node = findNode(root_, key);
    if (!node)
    {
      throw std::out_of_range("Key not found");
    }
    return node->value;
  }

  template < typename Key, typename Value, typename Compare >
  const Value& UBstTree< Key, Value, Compare >::at(const Key& key) const
  {
    const Node< Key, Value >* node = findNode(root_, key);
    if (!node)
    {
      throw std::out_of_range("Key not found");
    }
    return node->value;
  }

  template < typename Key, typename Value, typename Compare >
  typename UBstTree< Key, Value, Compare >::const_iterator UBstTree< Key, Value, Compare >::cbegin() const noexcept
  {
    return const_iterator(minNode(root_));
  }

  template < typename Key, typename Value, typename Compare >
  typename UBstTree< Key, Value, Compare >::const_iterator UBstTree< Key, Value, Compare >::cend() const noexcept
  {
    return const_iterator(nullptr);
  }

  template < typename Key, typename Value, typename Compare >
  typename UBstTree< Key, Value, Compare >::const_reverse_iterator UBstTree< Key, Value, Compare >::crbegin() const noexcept
  {
    return const_reverse_iterator(cend());
  }

  template < typename Key, typename Value, typename Compare >
  typename UBstTree< Key, Value, Compare >::const_reverse_iterator UBstTree< Key, Value, Compare >::crend() const noexcept
  {
    return const_reverse_iterator(cbegin());
  }

  template < typename Key, typename Value, typename Compare >
  typename UBstTree< Key, Value, Compare >::const_iterator UBstTree< Key, Value, Compare >::find(const Key& key) const noexcept
  {
    return const_iterator(findNode(root_, key));
  }

  template < typename Key, typename Value, typename Compare >
  template < typename F >
  F UBstTree< Key, Value, Compare >::traverse_lnr(F f) const
  {
    std::stack< const Node< Key, Value >* > stack;
    const Node< Key, Value >* current = root_;
    while (current || !stack.empty())
    {
      while (current)
      {
        stack.push(current);
        current = current->left;
      }
      current = stack.top();
      stack.pop();
      f(value_type{current->key, current->value});
      current = current->right;
    }
    return f;
  }

  template < typename Key, typename Value, typename Compare >
  template < typename F >
  F UBstTree< Key, Value, Compare >::traverse_rnl(F f) const
  {
    std::stack< const Node< Key, Value >* > stack;
    const Node< Key, Value >* current = root_;
    while (current || !stack.empty())
    {
      while (current)
      {
        stack.push(current);
        current = current->right;
      }
      current = stack.top();
      stack.pop();
      f(value_type{current->key, current->value});
      current = current->left;
    }
    return f;
  }

  template < typename Key, typename Value, typename Compare >
  template < typename F >
  F UBstTree< Key, Value, Compare >::traverse_breadth(F f) const
  {
    if (!root_)
    {
      return f;
    }
    std::queue< const Node< Key, Value >* > queue;
    queue.push(root_);
    while (!queue.empty())
    {
      const Node< Key, Value >* current = queue.front();
      queue.pop();
      f(value_type{current->key, current->value});
      if (current->left)
      {
        queue.push(current->left);
      }
      if (current->right)
      {
        queue.push(current->right);
      }
    }
    return f;
  }

  template < typename Key, typename Value, typename Compare >
  Node< Key, Value >* UBstTree< Key, Value, Compare >::findNode(Node< Key, Value >* node, const Key& key)
  {
    if (!node)
    {
      return nullptr;
    }
    if (comp_(key, node->key))
    {
      return findNode(node->left, key);
    }
    else if (comp_(node->key, key))
    {
      return findNode(node->right, key);
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  const Node< Key, Value >* UBstTree< Key, Value, Compare >::findNode(const Node< Key, Value >* node, const Key& key) const
  {
    if (!node)
    {
      return nullptr;
    }
    if (comp_(key, node->key))
    {
      return findNode(node->left, key);
    }
    else if (comp_(node->key, key))
    {
      return findNode(node->right, key);
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  const Node< Key, Value >* UBstTree< Key, Value, Compare >::minNode(const Node< Key, Value >* node) const
  {
    if (!node)
    {
      return nullptr;
    }
    while (node->left)
    {
      node = node->left;
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  const Node< Key, Value >* UBstTree< Key, Value, Compare >::maxNode(const Node< Key, Value >* node) const
  {
    if (!node)
    {
      return nullptr;
    }
    while (node->right)
    {
      node = node->right;
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  void UBstTree< Key, Value, Compare >::clearNode(Node< Key, Value >* node)
  {
    if (!node)
    {
      return;
    }
    clearNode(node->left);
    clearNode(node->right);
    delete node;
  }

  template < typename Key, typename Value, typename Compare >
  Node< Key, Value >* UBstTree< Key, Value, Compare >::insertNode(Node< Key, Value >* node,
    const Key& key, const Value& value, Node< Key, Value >* parent, size_t& size)
  {
    if (!node)
    {
      Node< Key, Value >* newNode = new Node< Key, Value >(key, value);
      newNode->parent = parent;
      ++size;
      return newNode;
    }
    if (comp_(key, node->key))
    {
      node->left = insertNode(node->left, key, value, node, size);
    }
    else if (comp_(node->key, key))
    {
      node->right = insertNode(node->right, key, value, node, size);
    }
    else
    {
      node->value = value;
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  void UBstTree< Key, Value, Compare >::copyTree(Node< Key, Value >*& node,
    Node< Key, Value >* otherNode,
    Node< Key, Value >* parent)
  {
    if (!otherNode)
    {
      node = nullptr;
      return;
    }
    node = new Node< Key, Value >(otherNode->key, otherNode->value);
    node->parent = parent;
    ++size_;
    try
    {
      copyTree(node->left, otherNode->left, node);
    }
    catch (std::bad_alloc&)
    {
      delete node;
      node = nullptr;
      --size_;
      throw;
    }
    try
    {
      copyTree(node->right, otherNode->right, node);
    }
    catch (std::bad_alloc&)
    {
      clearNode(node->left);
      delete node;
      node = nullptr;
      --size_;
      throw;
    }
  }

  template < typename Key, typename Value, typename Compare >
  size_t UBstTree< Key, Value, Compare >::erase(const Key& key)
  {
    root_ = eraseNode(root_, key, size_);
    return 1;
  }

  template < typename Key, typename Value, typename Compare >
  Node< Key, Value >* UBstTree< Key, Value, Compare >::eraseNode(Node< Key, Value >* node, const Key& key, size_t& size)
  {
    if (!node)
    {
      return node;
    }

    if (comp_(key, node->key))
    {
      node->left = eraseNode(node->left, key, size);
    }
    else if (comp_(node->key, key))
    {
      node->right = eraseNode(node->right, key, size);
    }
    else
    {
      Node< Key, Value >* temp = nullptr;
      if (!node->left)
      {
        temp = node->right;
      }
      else if (!node->right)
      {
        temp = node->left;
      }
      else
      {
        Node< Key, Value >* minRight = const_cast<Node< Key, Value >*>(minNode(node->right));
        if (minRight->parent != node)
        {
          minRight->parent->left = minRight->right;
          if (minRight->right)
          {
            minRight->right->parent = minRight->parent;
          }
          minRight->right = node->right;
          node->right->parent = minRight;
        }
        minRight->left = node->left;
        if (node->left)
        {
          node->left->parent = minRight;
        }
        temp = minRight;
      }
      if (temp)
      {
        temp->parent = node->parent;
      }
      delete node;
      --size;
      return temp;
    }
    return node;
  }
}

#endif
