#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <stdexcept>

namespace kushekbaev
{
  template< typename T >
  class RAII
  {
    public:
      explicit RAII(T* p);
      ~RAII();
      RAII(RAII&& other) noexcept;
      T* ptr;
  };

  template< typename T >
  RAII< T >::RAII(T* p):
    ptr(p)
  {}

  template< typename T >
  RAII< T >::~RAII()
  {
    delete[] ptr;
  }

  template< typename T >
  RAII< T >::RAII(RAII&& other) noexcept:
    ptr(other.ptr)
  {
    other.ptr = nullptr;
  }

  template< typename T >
  class Vector
  {
    public:
      Vector();
      Vector(const Vector& other);
      Vector(Vector&& other);
      ~Vector();

      Vector& operator=(const Vector& other);

      T& front() noexcept;
      T& back() noexcept;
      const T& front() const noexcept;
      const T& back() const noexcept;

      bool empty() const noexcept;
      size_t size() const noexcept;

      void push_back(const T& value);
      void pop_front();
      void pop_back();

      T* begin() noexcept;
      T* end() noexcept;
      const T* begin() const noexcept;
      const T* end() const noexcept;
      const T* cbegin() const noexcept;
      const T* cend() const noexcept;

    private:
      T* data_;
      size_t size_;
      size_t capacity_;
      void extend_vector();
  };

  template< typename T >
  Vector< T >::Vector():
    data_(new T[1]),
    size_(0),
    capacity_(1)
  {}

  template< typename T >
  Vector< T >::Vector(const Vector& other):
    data_(new T[other.capacity_]),
    size_(other.size_),
    capacity_(other.capacity_)
  {
    for (size_t i = 0; i < size_; ++i)
    {
      data_[i] = other.data_[i];
    }
  }

  template< typename T >
  Vector< T >::Vector(Vector&& other):
    data_(other.data_),
    size_(other.size_),
    capacity_(other.capacity_)
  {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }

  template< typename T >
  Vector< T >::~Vector()
  {
    delete[] data_;
  }

  template< typename T >
  Vector< T >& Vector< T >::operator=(const Vector& other)
  {
    if (this != &other)
    {
      T* newData = new T[other.capacity_];
      for (size_t i = 0; i < other.size_; ++i)
      {
        newData[i] = other.data_[i];
      }
      delete[] data_;
      data_ = newData;
      size_ = other.size_;
      capacity_ = other.capacity_;
    }
    return *this;
  }

  template< typename T >
  bool Vector< T >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< typename T >
  size_t Vector< T >::size() const noexcept
  {
    return size_;
  }

  template< typename T >
  T& Vector< T >::front() noexcept
  {
    return data_[0];
  }

  template< typename T >
  T& Vector< T >::back() noexcept
  {
    return data_[size_ - 1];
  }

  template< typename T >
  const T& Vector< T >::front() const noexcept
  {
    return data_[0];
  }

  template< typename T >
  const T& Vector< T >::back() const noexcept
  {
    return data_[size_ - 1];
  }

  template< typename T >
  void Vector< T >::push_back(const T& value)
  {
    if (size_ == capacity_)
    {
      extend_vector();
    }
    data_[size_++] = value;
  }

  template< typename T >
  void Vector< T >::pop_front()
  {
    if (empty())
    {
      throw std::out_of_range("The array is empty!");
    }
    for (size_t i = 1; i < size_; ++i)
    {
      data_[i - 1] = std::move(data_[i]);
    }
    --size_;
  }

  template< typename T >
  void Vector< T >::pop_back()
  {
    if (!empty())
    {
      --size_;
    }
  }

  template< typename T >
  void Vector< T >::extend_vector()
  {
    size_t newCapacity = (capacity_ == 0) ? 1 : capacity_ * 2;
    RAII< T > newHolder(new T[newCapacity]);
    for (size_t i = 0; i < size_; ++i)
    {
      newHolder.ptr[i] = std::move(data_[i]);
    }
    delete[] data_;
    data_ = newHolder.ptr;
    newHolder.ptr = nullptr;
    capacity_ = newCapacity;
  }

  template< typename T >
  T* Vector< T >::begin() noexcept
  {
    return data_;
  }

  template< typename T >
  T* Vector< T >::end() noexcept
  {
    return data_ + size_;
  }

  template< typename T >
  const T* Vector< T >::begin() const noexcept
  {
    return data_;
  }

  template< typename T >
  const T* Vector< T >::end() const noexcept
  {
    return data_ + size_;
  }

  template< typename T >
  const T* Vector< T >::cbegin() const noexcept
  {
    return data_;
  }

  template< typename T >
  const T* Vector< T >::cend() const noexcept
  {
    return data_ + size_;
  }
}

#endif
