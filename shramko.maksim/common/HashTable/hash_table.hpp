#ifndef SHRAMKO_HASH_TABLE_HPP
#define SHRAMKO_HASH_TABLE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>
#include <iterator>

namespace shramko {

template <typename Key, typename T, typename Hash = std::hash<Key>, typename Eq = std::equal_to<Key>>
class HashTable {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = std::size_t;
    using hasher = Hash;
    using key_equal = Eq;

    class iterator;
    class cIterator;

private:
    struct Slot {
        Key key;
        T data;
        bool occupied = false;
        bool deleted = false;
    };

    std::vector<Slot> slots_;
    size_type size_ = 0;
    size_type capacity_ = 0;
    float max_load_factor_ = 0.75f;
    Hash hash_;
    Eq eq_;

    size_type hash(const Key& key) const {
        return hash_(key) % capacity_;
    }

    bool equal(const Key& lhs, const Key& rhs) const {
        return eq_(lhs, rhs);
    }

    void allocate_slots(size_type new_capacity) {
        capacity_ = new_capacity;
        slots_.resize(capacity_);
        for (auto& slot : slots_) {
            slot.occupied = false;
            slot.deleted = false;
        }
    }

public:
    HashTable() : HashTable(16) {}

    explicit HashTable(size_type bucket_count)
        : capacity_(bucket_count), max_load_factor_(0.75f) {
        if (bucket_count == 0) {
            throw std::invalid_argument("Bucket count must be positive");
        }
        allocate_slots(bucket_count);
    }

    HashTable(const HashTable& rhs)
        : HashTable(rhs.capacity_) {
        for (size_type i = 0; i < capacity_; ++i) {
            if (rhs.slots_[i].occupied && !rhs.slots_[i].deleted) {
                slots_[i].key = rhs.slots_[i].key;
                slots_[i].data = rhs.slots_[i].data;
                slots_[i].occupied = true;
                slots_[i].deleted = false;
                ++size_;
            }
        }
    }

    HashTable(HashTable&& rhs) noexcept
        : size_(rhs.size_), capacity_(rhs.capacity_),
          slots_(std::move(rhs.slots_)), max_load_factor_(rhs.max_load_factor_),
          hash_(std::move(rhs.hash_)), eq_(std::move(rhs.eq_)) {
        rhs.size_ = 0;
        rhs.capacity_ = 0;
        rhs.max_load_factor_ = 0.75f;
    }

    ~HashTable() = default;

    HashTable& operator=(const HashTable& rhs) {
        if (this == &rhs) return *this;
        HashTable tmp(rhs);
        swap(tmp);
        return *this;
    }

    HashTable& operator=(HashTable&& rhs) noexcept {
        if (this == &rhs) return *this;
        clear();
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        slots_ = std::move(rhs.slots_);
        max_load_factor_ = rhs.max_load_factor_;
        hash_ = std::move(rhs.hash_);
        eq_ = std::move(rhs.eq_);
        rhs.size_ = 0;
        rhs.capacity_ = 0;
        rhs.max_load_factor_ = 0.75f;
        return *this;
    }

    T& operator[](const Key& key) {
        auto [it, inserted] = insert_or_assign(key, T{});
        return it->second;
    }

    const T& at(const Key& key) const {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("Key not found");
        }
        return it->second;
    }

    T& at(const Key& key) {
        auto it = find(key);
        if (it == end()) {
            throw std::out_of_range("Key not found");
        }
        return it->second;
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        if (loadFactor() > max_load_factor_) {
            rehash(capacity_ * 2);
        }
        size_type index = hash(value.first);
        size_type first_deleted = capacity_;
        for (size_type i = 0; i < capacity_; ++i) {
            size_type pos = (index + i) % capacity_;
            if (!slots_[pos].occupied) {
                if (first_deleted != capacity_) pos = first_deleted;
                slots_[pos].key = value.first;
                slots_[pos].data = value.second;
                slots_[pos].occupied = true;
                slots_[pos].deleted = false;
                ++size_;
                return {iterator(slots_.data() + pos, slots_.data() + capacity_), true};
            }
            if (slots_[pos].deleted && first_deleted == capacity_) {
                first_deleted = pos;
            }
            if (slots_[pos].occupied && equal(slots_[pos].key, value.first)) {
                return {iterator(slots_.data() + pos, slots_.data() + capacity_), false};
            }
        }
        throw std::overflow_error("Hash table overflow");
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        if (loadFactor() > max_load_factor_) {
            rehash(capacity_ * 2);
        }
        size_type index = hash(value.first);
        size_type first_deleted = capacity_;
        for (size_type i = 0; i < capacity_; ++i) {
            size_type pos = (index + i) % capacity_;
            if (!slots_[pos].occupied) {
                if (first_deleted != capacity_) pos = first_deleted;
                slots_[pos].key = std::move(value.first);
                slots_[pos].data = std::move(value.second);
                slots_[pos].occupied = true;
                slots_[pos].deleted = false;
                ++size_;
                return {iterator(slots_.data() + pos, slots_.data() + capacity_), true};
            }
            if (slots_[pos].deleted && first_deleted == capacity_) {
                first_deleted = pos;
            }
            if (slots_[pos].occupied && equal(slots_[pos].key, value.first)) {
                return {iterator(slots_.data() + pos, slots_.data() + capacity_), false};
            }
        }
        throw std::overflow_error("Hash table overflow");
    }

    std::pair<iterator, bool> insert(const Key& key, T&& obj) {
        return emplace(key, std::forward<T>(obj));
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        value_type value(std::forward<Args>(args)...);
        return insert(std::move(value));
    }

    template <typename... Args>
    std::pair<iterator, bool> emplace(const Key& key, Args&&... args) {
        if (loadFactor() > max_load_factor_) {
            rehash(capacity_ * 2);
        }
        T obj(std::forward<Args>(args)...);
        size_type index = hash(key);
        size_type first_deleted = capacity_;
        for (size_type i = 0; i < capacity_; ++i) {
            size_type pos = (index + i) % capacity_;
            if (!slots_[pos].occupied) {
                if (first_deleted != capacity_) pos = first_deleted;
                slots_[pos].key = key;
                slots_[pos].data = std::move(obj);
                slots_[pos].occupied = true;
                slots_[pos].deleted = false;
                ++size_;
                return {iterator(slots_.data() + pos, slots_.data() + capacity_), true};
            }
            if (slots_[pos].deleted && first_deleted == capacity_) {
                first_deleted = pos;
            }
            if (slots_[pos].occupied && equal(slots_[pos].key, key)) {
                slots_[pos].data = std::move(obj);
                return {iterator(slots_.data() + pos, slots_.data() + capacity_), false};
            }
        }
        throw std::overflow_error("Hash table overflow");
    }

    template <typename... Args>
    iterator insert_or_assign(const Key& key, Args&&... args) {
        auto [it, inserted] = emplace(key, std::forward<Args>(args)...);
        if (!inserted) {
            it->second = T(std::forward<Args>(args)...);
        }
        return it;
    }

    iterator erase(iterator pos) {
        if (pos == end()) return end();
        pos.base()->occupied = false;
        pos.base()->deleted = true;
        --size_;
        return ++pos;
    }

    size_type erase(const Key& key) {
        auto it = find(key);
        if (it == end()) return 0;
        erase(it);
        return 1;
    }

    void clear() noexcept {
        for (auto& slot : slots_) {
            slot.occupied = false;
            slot.deleted = false;
        }
        size_ = 0;
    }

    void swap(HashTable& rhs) noexcept {
        std::swap(size_, rhs.size_);
        std::swap(capacity_, rhs.capacity_);
        std::swap(slots_, rhs.slots_);
        std::swap(max_load_factor_, rhs.max_load_factor_);
        std::swap(hash_, rhs.hash_);
        std::swap(eq_, rhs.eq_);
    }

    float loadFactor() const noexcept {
        return static_cast<float>(size_) / capacity_;
    }

    float max_load_factor() const noexcept {
        return max_load_factor_;
    }

    void max_load_factor(float ml) {
        if (ml <= 0.0f || ml > 1.0f) {
            throw std::invalid_argument("Max load factor must be in (0, 1]");
        }
        max_load_factor_ = ml;
        if (loadFactor() > max_load_factor_) {
            rehash(static_cast<size_type>(capacity_ * 2));
        }
    }

    void reserve(size_type n) {
        if (static_cast<float>(n) > max_load_factor_ * capacity_) {
            rehash(static_cast<size_type>(n / max_load_factor_ + 1));
        }
    }

    void rehash(size_type n) {
        if (n <= capacity_) return;
        HashTable tmp(n);
        for (auto& slot : slots_) {
            if (slot.occupied && !slot.deleted) {
                tmp.insert({slot.key, slot.data});
            }
        }
        swap(tmp);
    }

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    size_type bucket_count() const noexcept { return capacity_; }

    iterator find(const Key& key) {
        size_type index = hash(key);
        for (size_type i = 0; i < capacity_; ++i) {
            size_type pos = (index + i) % capacity_;
            if (!slots_[pos].occupied) break;
            if (slots_[pos].deleted) continue;
            if (equal(slots_[pos].key, key)) {
                return iterator(slots_.data() + pos, slots_.data() + capacity_);
            }
        }
        return end();
    }

    cIterator find(const Key& key) const {
        size_type index = hash(key);
        for (size_type i = 0; i < capacity_; ++i) {
            size_type pos = (index + i) % capacity_;
            if (!slots_[pos].occupied) break;
            if (slots_[pos].deleted) continue;
            if (equal(slots_[pos].key, key)) {
                return cIterator(slots_.data() + pos, slots_.data() + capacity_);
            }
        }
        return end();
    }

    iterator begin() noexcept {
        return iterator(slots_.data(), slots_.data() + capacity_);
    }

    iterator end() noexcept {
        return iterator(slots_.data() + capacity_, slots_.data() + capacity_);
    }

    cIterator begin() const noexcept {
        return cIterator(slots_.data(), slots_.data() + capacity_);
    }

    cIterator end() const noexcept {
        return cIterator(slots_.data() + capacity_, slots_.data() + capacity_);
    }

    cIterator cbegin() const noexcept {
        return begin();
    }

    cIterator cend() const noexcept {
        return end();
    }

    class iterator {
    private:
        Slot* current_;
        Slot* end_;
        const HashTable* table_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = HashTable::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator() : current_(nullptr), end_(nullptr), table_(nullptr) {}

        iterator(Slot* start, Slot* end, const HashTable& table) : current_(start), end_(end), table_(&table) {
            advance_to_next();
        }

        reference operator*() const { return deref(); }
        pointer operator->() const { return &deref(); }

        iterator& operator++() {
            ++current_;
            advance_to_next();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return current_ == other.current_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        void advance_to_next() {
            while (current_ != end_ && (!current_->occupied || current_->deleted)) {
                ++current_;
            }
        }

        value_type& deref() const {
            static value_type dummy;
            if (current_ == end_ || !current_->occupied || current_->deleted) return dummy;
            return *(new value_type(current_->key, current_->data));
        }

        Slot* base() const { return current_; }
    };

    class cIterator {
    private:
        const Slot* current_;
        const Slot* end_;
        const HashTable* table_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const HashTable::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        cIterator() : current_(nullptr), end_(nullptr), table_(nullptr) {}

        cIterator(const Slot* start, const Slot* end, const HashTable& table) : current_(start), end_(end), table_(&table) {
            advance_to_next();
        }

        reference operator*() const { return deref(); }
        pointer operator->() const { return &deref(); }

        cIterator& operator++() {
            ++current_;
            advance_to_next();
            return *this;
        }

        cIterator operator++(int) {
            cIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const cIterator& other) const { return current_ == other.current_; }
        bool operator!=(const cIterator& other) const { return !(*this == other); }

    private:
        void advance_to_next() {
            while (current_ != end_ && (!current_->occupied || current_->deleted)) {
                ++current_;
            }
        }

        value_type& deref() const {
            static value_type dummy;
            if (current_ == end_ || !current_->occupied || current_->deleted) return dummy;
            return *(new value_type(current_->key, current_->data));
        }
    };
};

template <typename Key, typename T, typename Hash, typename Eq>
void swap(HashTable<Key, T, Hash, Eq>& lhs, HashTable<Key, T, Hash, Eq>& rhs) noexcept {
    lhs.swap(rhs);
}

}

#endif
