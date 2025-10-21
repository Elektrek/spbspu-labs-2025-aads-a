#ifndef FORWARDLIST_HPP
#define FORWARDLIST_HPP

#include <cassert>
#include <cstddef>
#include <utility>
#include <stdexcept>
#include <functional>

#include "fwd_list_node.hpp"
#include "fwd_const_iterator.hpp"
#include "fwd_iterator.hpp"

namespace shramko
{
  template< typename T >
  class ForwardList
  {
  public:
    friend class FwdConstIterator< T >;
    friend class FwdIterator< T >;

    using const_iterator = FwdConstIterator< T >;
    using iterator = FwdIterator< T >;

    ForwardList();
    ~ForwardList() noexcept;
    ForwardList(const ForwardList< T >& other);
    ForwardList(ForwardList< T >&& other) noexcept;
    ForwardList< T >& operator=(const ForwardList< T >& other);
    ForwardList< T >& operator=(ForwardList< T >&& other) noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    const T& getFront() const;
    const T& getBack() const;
    T& getFront();
    T& getBack();

    bool isEmpty() const noexcept;
    size_t getSize() const noexcept;

    void addToFront(const T& value);
    void addToFront(T&& value);
    void removeFront();

    void swapLists(ForwardList< T >& other) noexcept;
    void clearAll() noexcept;

    void addToBack(const T& value);
    void addToBack(T&& value);

    template <class Comp = std::less<T>>
    void sort(Comp comp = Comp());

  private:
    ListNode< T >* headNode_;
    ListNode< T >* tailNode_;
    size_t currentSize_;

    void insertFrontNode(ListNode< T >* newNode) noexcept;
    void insertBackNode(ListNode< T >* newNode) noexcept;
  };
}

template< typename T >
shramko::ForwardList< T >::ForwardList():
  headNode_(nullptr),
  tailNode_(nullptr),
  currentSize_(0)
{}

template< typename T >
shramko::ForwardList< T >::ForwardList(const ForwardList< T >& other):
  headNode_(nullptr),
  tailNode_(nullptr),
  currentSize_(0)
{
  for (auto it = other.begin(); it != other.end(); ++it)
  {
    addToBack(*it);
  }
}

template< typename T >
shramko::ForwardList< T >::ForwardList(ForwardList< T >&& other) noexcept:
  headNode_(other.headNode_),
  tailNode_(other.tailNode_),
  currentSize_(other.currentSize_)
{
  other.headNode_ = other.tailNode_ = nullptr;
  other.currentSize_ = 0;
}

template< typename T >
shramko::ForwardList< T >& shramko::ForwardList< T >::operator=(const ForwardList< T >& other)
{
  if (this != &other)
  {
    ForwardList< T > temp(other);
    swapLists(temp);
  }
  return *this;
}

template< typename T >
shramko::ForwardList< T >& shramko::ForwardList< T >::operator=(ForwardList< T >&& other) noexcept
{
  if (this != &other)
  {
    clearAll();
    headNode_ = other.headNode_;
    tailNode_ = other.tailNode_;
    currentSize_ = other.currentSize_;
    other.headNode_ = other.tailNode_ = nullptr;
    other.currentSize_ = 0;
  }
  return *this;
}

template< typename T >
shramko::ForwardList< T >::~ForwardList() noexcept
{
  clearAll();
}

template< typename T >
typename shramko::ForwardList< T >::iterator shramko::ForwardList< T >::begin() noexcept
{
  return iterator(headNode_);
}

template< typename T >
typename shramko::ForwardList< T >::iterator shramko::ForwardList< T >::end() noexcept
{
  return iterator(nullptr);
}

template< typename T >
typename shramko::ForwardList< T >::const_iterator shramko::ForwardList< T >::begin() const noexcept
{
  return const_iterator(headNode_);
}

template< typename T >
typename shramko::ForwardList< T >::const_iterator shramko::ForwardList< T >::end() const noexcept
{
  return const_iterator(nullptr);
}

template< typename T >
typename shramko::ForwardList< T >::const_iterator shramko::ForwardList< T >::cbegin() const noexcept
{
  return const_iterator(headNode_);
}

template< typename T >
typename shramko::ForwardList< T >::const_iterator shramko::ForwardList< T >::cend() const noexcept
{
  return const_iterator(nullptr);
}

template< typename T >
const T& shramko::ForwardList< T >::getFront() const
{
  if (isEmpty())
  {
    throw std::logic_error("List is empty!");
  }
  return headNode_->dataValue;
}

template< typename T >
const T& shramko::ForwardList< T >::getBack() const
{
  if (isEmpty())
  {
    throw std::logic_error("List is empty!");
  }
  return tailNode_->dataValue;
}

template< typename T >
T& shramko::ForwardList< T >::getFront()
{
  if (isEmpty())
  {
    throw std::logic_error("List is empty!");
  }
  return headNode_->dataValue;
}

template< typename T >
T& shramko::ForwardList< T >::getBack()
{
  if (isEmpty())
  {
    throw std::logic_error("List is empty!");
  }
  return tailNode_->dataValue;
}

template< typename T >
bool shramko::ForwardList< T >::isEmpty() const noexcept
{
  return currentSize_ == 0;
}

template< typename T >
size_t shramko::ForwardList< T >::getSize() const noexcept
{
  return currentSize_;
}

template< typename T >
void shramko::ForwardList< T >::insertFrontNode(ListNode< T >* newNode) noexcept
{
  newNode->nextPtr = headNode_;
  headNode_ = newNode;
  if (currentSize_ == 0)
  {
    tailNode_ = newNode;
  }
  currentSize_++;
}

template< typename T >
void shramko::ForwardList< T >::addToFront(const T& value)
{
  insertFrontNode(new ListNode< T >(value));
}

template< typename T >
void shramko::ForwardList< T >::addToFront(T&& value)
{
  insertFrontNode(new ListNode< T >(std::move(value)));
}

template< typename T >
void shramko::ForwardList< T >::removeFront()
{
  if (!isEmpty())
  {
    ListNode< T >* oldHead = headNode_;
    headNode_ = headNode_->nextPtr;
    delete oldHead;
    currentSize_--;
    if (currentSize_ == 0)
    {
      tailNode_ = nullptr;
    }
  }
}

template< typename T >
void shramko::ForwardList< T >::insertBackNode(ListNode< T >* newNode) noexcept
{
  if (currentSize_ == 0)
  {
    headNode_ = tailNode_ = newNode;
  }
  else
  {
    tailNode_->nextPtr = newNode;
    tailNode_ = newNode;
  }
  currentSize_++;
}

template< typename T >
void shramko::ForwardList< T >::addToBack(const T& value)
{
  insertBackNode(new ListNode< T >(value));
}

template< typename T >
void shramko::ForwardList< T >::addToBack(T&& value)
{
  insertBackNode(new ListNode< T >(std::move(value)));
}

template< typename T >
void shramko::ForwardList< T >::swapLists(ForwardList< T >& other) noexcept
{
  std::swap(headNode_, other.headNode_);
  std::swap(tailNode_, other.tailNode_);
  std::swap(currentSize_, other.currentSize_);
}

template< typename T >
void shramko::ForwardList< T >::clearAll() noexcept
{
  while (!isEmpty())
  {
    removeFront();
  }
}

template< typename T >
template <class Comp>
void shramko::ForwardList< T >::sort(Comp comp)
{
  if (getSize() < 2)
  {
    return;
  }

  bool swapped;
  do
  {
    swapped = false;
    auto prev = begin();
    auto curr = begin();
    ++curr;
    while (curr != end())
    {
      if (comp(*curr, *prev))
      {
        std::swap(*prev, *curr);
        swapped = true;
      }
      ++prev;
      ++curr;
    }
  } while (swapped);
}

#endif
