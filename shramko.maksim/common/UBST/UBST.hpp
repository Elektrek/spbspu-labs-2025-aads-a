#ifndef SHRAMKO_UBST_HPP
#define SHRAMKO_UBST_HPP

#include <functional>
#include <memory>
#include <utility>
#include <queue>
#include <stack>
#include <stdexcept>

namespace shramko {

template <typename Key, typename Value, typename Compare = std::less<Key>>
class UBstTree {
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using size_type = std::size_t;
    using key_compare = Compare;

    class const_iterator;

private:
    struct Node {
        Key key;
        Value value;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        Node(const Key& k, const Value& v) : key(k), value(v) {}
        Node(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) {}
    };

    Node* root_ = nullptr;
    key_compare comp_{};
    size_type size_ = 0;

    Node* findNode(Node* node, const Key& key) const {
        if (node == nullptr) return nullptr;
        if (comp_(key, node->key)) {
            return findNode(node->left, key);
        } else if (comp_(node->key, key)) {
            return findNode(node->right, key);
        }
        return node;
    }

    Node* insertNode(Node* node, Node* parent, value_type&& val) {
        if (node == nullptr) {
            Node* newNode = new Node(std::move(val.first), std::move(val.second));
            newNode->parent = parent;
            ++size_;
            return newNode;
        }
        if (comp_(val.first, node->key)) {
            node->left = insertNode(node->left, node, std::move(val));
        } else if (comp_(node->key, val.first)) {
            node->right = insertNode(node->right, node, std::move(val));
        } else {
            node->value = std::move(val.second);
        }
        return node;
    }

    Node* minNode(Node* node) const {
        while (node && node->left) node = node->left;
        return node;
    }

    Node* maxNode(Node* node) const {
        while (node && node->right) node = node->right;
        return node;
    }

    void deleteNode(Node* node) {
        if (node == nullptr) return;
        if (node->left == nullptr) {
            transplant(node, node->right);
        } else if (node->right == nullptr) {
            transplant(node, node->left);
        } else {
            Node* minRight = minNode(node->right);
            if (minRight->parent != node) {
                transplant(minRight, minRight->right);
                minRight->right = node->right;
                minRight->right->parent = minRight;
            }
            transplant(node, minRight);
            minRight->left = node->left;
            minRight->left->parent = minRight;
            minRight->key = std::move(node->key);
            minRight->value = std::move(node->value);
        }
        --size_;
        delete node;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr) {
            root_ = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        if (v != nullptr) v->parent = u->parent;
    }

public:
    UBstTree() = default;

    UBstTree(const UBstTree& other) {
        root_ = copyTree(other.root_);
        size_ = other.size_;
    }

    UBstTree(UBstTree&& other) noexcept : root_(other.root_), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    ~UBstTree() {
        clear();
    }

    UBstTree& operator=(const UBstTree& other) {
        if (this != &other) {
            clear();
            root_ = copyTree(other.root_);
            size_ = other.size_;
        }
        return *this;
    }

    UBstTree& operator=(UBstTree&& other) noexcept {
        if (this != &other) {
            clear();
            root_ = other.root_;
            size_ = other.size_;
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    Value& operator[](const Key& key) {
        Node* node = findNode(root_, key);
        if (node == nullptr) {
            root_ = insertNode(root_, nullptr, {key, Value{}});
            node = findNode(root_, key);
        }
        return node->value;
    }

    const Value& at(const Key& key) const {
        Node* node = findNode(root_, key);
        if (node == nullptr) {
            throw std::out_of_range("Key not found");
        }
        return node->value;
    }

    void insert(const value_type& val) {
        root_ = insertNode(root_, nullptr, val);
    }

    void insert(value_type&& val) {
        root_ = insertNode(root_, nullptr, std::move(val));
    }

    size_type erase(const Key& key) {
        Node* node = findNode(root_, key);
        if (node != nullptr) {
            deleteNode(node);
            return 1;
        }
        return 0;
    }

    bool contains(const Key& key) const {
        return findNode(root_, key) != nullptr;
    }

    size_type size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    void clear() noexcept {
        clearTree(root_);
        root_ = nullptr;
        size_ = 0;
    }

    template <typename F>
    F traverse_lnr(F f) const {
        std::stack<Node*> stk;
        Node* current = root_;
        while (current || !stk.empty()) {
            while (current) {
                stk.push(current);
                current = current->left;
            }
            current = stk.top();
            stk.pop();
            f({current->key, current->value});
            current = current->right;
        }
        return f;
    }

    template <typename F>
    F traverse_rnl(F f) const {
        std::stack<Node*> stk;
        Node* current = root_;
        while (current || !stk.empty()) {
            while (current) {
                stk.push(current);
                current = current->right;
            }
            current = stk.top();
            stk.pop();
            f({current->key, current->value});
            current = current->left;
        }
        return f;
    }

    template <typename F>
    F traverse_breadth(F f) const {
        if (root_ == nullptr) return f;
        std::queue<Node*> q;
        q.push(root_);
        while (!q.empty()) {
            Node* current = q.front();
            q.pop();
            f({current->key, current->value});
            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
        return f;
    }

    const_iterator find(const Key& key) const {
        return const_iterator(findNode(root_, key));
    }

    const_iterator begin() const {
        if (root_ == nullptr) return end();
        Node* cur = root_;
        while (cur->left) cur = cur->left;
        return const_iterator(cur);
    }

    const_iterator end() const {
        return const_iterator(nullptr);
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

private:
    Node* copyTree(Node* node) const {
        if (node == nullptr) return nullptr;
        Node* newNode = new Node(node->key, node->value);
        newNode->left = copyTree(node->left);
        if (newNode->left) newNode->left->parent = newNode;
        newNode->right = copyTree(node->right);
        if (newNode->right) newNode->right->parent = newNode;
        return newNode;
    }

    void clearTree(Node* node) noexcept {
        if (node == nullptr) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

public:
    class const_iterator {
    private:
        const Node* current;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<const Key, Value>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        const_iterator() : current(nullptr) {}
        const_iterator(const Node* n) : current(n) {}

        reference operator*() const { return {current->key, current->value}; }
        pointer operator->() const { return &**this; }

        const_iterator& operator++() {
            if (current == nullptr) return *this;
            if (current->right) {
                current = current->right;
                while (current->left) current = current->left;
            } else {
                const Node* p = current->parent;
                while (p && current == p->right) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator& operator--() {
            if (current == nullptr) return *this;
            if (current->left) {
                current = current->left;
                while (current->right) current = current->right;
            } else {
                const Node* p = current->parent;
                while (p && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return current == other.current; }
        bool operator!=(const const_iterator& other) const { return !(*this == other); }
    };
};

}

#endif
