#ifndef UBST_HPP
#define UBST_HPP

#include <cstddef>
#include <stdexcept>
#include <stack>
#include <queue>
#include <new>
#include <utility>  // for std::swap
#include "node.hpp"
#include "constiterator.hpp"

namespace shramko
{
  template < typename Key, typename Value, typename Compare = std::less< Key > >
  class UBstTree
  {
  public:
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
    size_t erase(const Key& key);

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    const_iterator find(const Key& key) const noexcept;

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

    void copyTree(Node< Key, Value >*& node, Node< Key, Value >* otherNode, Node< Key, Value >* parent);

    Node< Key, Value >* findNodeNonConst(Node< Key, Value >* node, const Key& key);
    const Node< Key, Value >* findNodeConst(const Node< Key, Value >* node, const Key& key) const;
    Node< Key, Value >* minNode(Node< Key, Value >* node) const;
  };

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::UBstTree() : root_(nullptr), size_(0) {}

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::UBstTree(const UBstTree& other) : root_(nullptr), size_(0)
  {
    copyTree(root_, other.root_, nullptr);
    size_ = other.size_;
  }

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >::UBstTree(UBstTree&& other) noexcept : root_(other.root_), size_(other.size_)
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
    if (this != &other)
    {
      clear();
      copyTree(root_, other.root_, nullptr);
      size_ = other.size_;
    }
    return *this;
  }

  template < typename Key, typename Value, typename Compare >
  UBstTree< Key, Value, Compare >& UBstTree< Key, Value, Compare >::operator=(UBstTree&& other) noexcept
  {
    if (this != &other)
    {
      clear();
      root_ = other.root_;
      size_ = other.size_;
      other.root_ = nullptr;
      other.size_ = 0;
    }
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
  }

  template < typename Key, typename Value, typename Compare >
  Value& UBstTree< Key, Value, Compare >::operator[](const Key& key)
  {
    Node< Key, Value >* node = findNodeNonConst(root_, key);
    if (node)
    {
      return node->data.second;
    }
    size_t dummy = 0;
    root_ = insertNode(root_, key, Value{}, nullptr, dummy);
    ++size_;
    return root_->data.second;
  }

  template < typename Key, typename Value, typename Compare >
  const Value& UBstTree< Key, Value, Compare >::operator[](const Key& key) const
  {
    const Node< Key, Value >* node = findNodeConst(root_, key);
    if (node)
    {
      return node->data.second;
    }
    throw std::out_of_range("Key not found");
  }

  template < typename Key, typename Value, typename Compare >
  Value& UBstTree< Key, Value, Compare >::at(const Key& key)
  {
    Node< Key, Value >* node = findNodeNonConst(root_, key);
    if (node)
    {
      return node->data.second;
    }
    throw std::out_of_range("Key not found");
  }

  template < typename Key, typename Value, typename Compare >
  const Value& UBstTree< Key, Value, Compare >::at(const Key& key) const
  {
    const Node< Key, Value >* node = findNodeConst(root_, key);
    if (node)
    {
      return node->data.second;
    }
    throw std::out_of_range("Key not found");
  }

  template < typename Key, typename Value, typename Compare >
  typename UBstTree< Key, Value, Compare >::const_iterator UBstTree< Key, Value, Compare >::cbegin() const noexcept
  {
    return const_iterator(root_);
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
    return const_iterator(findNodeConst(root_, key));
  }

  template < typename Key, typename Value, typename Compare >
  template < typename F >
  F UBstTree< Key, Value, Compare >::traverse_lnr(F f) const
  {
    std::stack<const Node< Key, Value >*> stk;
    const Node< Key, Value >* curr = root_;
    while (curr || !stk.empty())
    {
      while (curr)
      {
        stk.push(curr);
        curr = curr->left;
      }
      curr = stk.top();
      stk.pop();
      f(curr->data);
      curr = curr->right;
    }
    return f;
  }

  template < typename Key, typename Value, typename Compare >
  template < typename F >
  F UBstTree< Key, Value, Compare >::traverse_rnl(F f) const
  {
    std::stack<const Node< Key, Value >*> stk;
    const Node< Key, Value >* curr = root_;
    while (curr || !stk.empty())
    {
      while (curr)
      {
        stk.push(curr);
        curr = curr->right;
      }
      curr = stk.top();
      stk.pop();
      f(curr->data);
      curr = curr->left;
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
    std::queue<const Node< Key, Value >*> q;
    q.push(root_);
    while (!q.empty())
    {
      const Node< Key, Value >* curr = q.front();
      q.pop();
      f(curr->data);
      if (curr->left)
      {
        q.push(curr->left);
      }
      if (curr->right)
      {
        q.push(curr->right);
      }
    }
    return f;
  }

  template < typename Key, typename Value, typename Compare >
  void UBstTree< Key, Value, Compare >::clearNode(Node< Key, Value >* node)
  {
    if (node)
    {
      clearNode(node->left);
      clearNode(node->right);
      delete node;
    }
  }

  template < typename Key, typename Value, typename Compare >
  Node< Key, Value >* UBstTree< Key, Value, Compare >::insertNode(Node< Key, Value >* node, const Key& key,
                                                                 const Value& value, Node< Key, Value >* parent, size_t& size)
  {
    if (!node)
    {
      node = new Node< Key, Value >{std::make_pair(key, value), parent, nullptr, nullptr};
      ++size;
      return node;
    }
    if (comp_(key, node->data.first))
    {
      node->left = insertNode(node->left, key, value, node, size);
    }
    else if (comp_(node->data.first, key))
    {
      node->right = insertNode(node->right, key, value, node, size);
    }
    else
    {
      node->data.second = value;
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  void UBstTree< Key, Value, Compare >::copyTree(Node< Key, Value >*& node, Node< Key, Value >* otherNode, Node< Key, Value >* parent)
  {
    if (otherNode)
    {
      try
      {
        node = new Node< Key, Value >{otherNode->data, parent, nullptr, nullptr};
        copyTree(node->left, otherNode->left, node);
        copyTree(node->right, otherNode->right, node);
      }
      catch (std::bad_alloc&)
      {
        clearNode(node->left);
        delete node;
        node = nullptr;
        throw;
      }
    }
  }

  template < typename Key, typename Value, typename Compare >
  Node< Key, Value >* UBstTree< Key, Value, Compare >::findNodeNonConst(Node< Key, Value >* node, const Key& key)
  {
    if (!node)
    {
      return nullptr;
    }
    if (comp_(key, node->data.first))
    {
      return findNodeNonConst(node->left, key);
    }
    else if (comp_(node->data.first, key))
    {
      return findNodeNonConst(node->right, key);
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  const Node< Key, Value >* UBstTree< Key, Value, Compare >::findNodeConst(const Node< Key, Value >* node, const Key& key) const
  {
    if (!node)
    {
      return nullptr;
    }
    if (comp_(key, node->data.first))
    {
      return findNodeConst(node->left, key);
    }
    else if (comp_(node->data.first, key))
    {
      return findNodeConst(node->right, key);
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  Node< Key, Value >* UBstTree< Key, Value, Compare >::minNode(Node< Key, Value >* node) const
  {
    while (node && node->left)
    {
      node = node->left;
    }
    return node;
  }

  template < typename Key, typename Value, typename Compare >
  size_t UBstTree< Key, Value, Compare >::erase(const Key& key)
  {
    Node< Key, Value >* toDel = findNodeNonConst(root_, key);
    if (!toDel)
    {
      return 0;
    }
    --size_;
    if (!toDel->left || !toDel->right)
    {
      Node< Key, Value >* child = toDel->left ? toDel->left : toDel->right;
      if (child)
      {
        child->parent = toDel->parent;
      }
      if (!toDel->parent)
      {
        root_ = child;
      }
      else if (toDel == toDel->parent->left)
      {
        toDel->parent->left = child;
      }
      else
      {
        toDel->parent->right = child;
      }
      delete toDel;
    }
    else
    {
      Node< Key, Value >* succ = minNode(toDel->right);
      std::swap(toDel->data, succ->data);  // Swap вместо assignment
      Node< Key, Value >* sparent = succ->parent;
      if (sparent->left == succ)
      {
        sparent->left = succ->right;
      }
      else
      {
        sparent->right = succ->right;
      }
      if (succ->right)
      {
        succ->right->parent = sparent;
      }
      delete succ;
    }
    return 1;
  }
}

#endif
