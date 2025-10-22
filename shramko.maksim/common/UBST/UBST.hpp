#ifndef SHRAMKO_UBST_HPP
#define SHRAMKO_UBST_HPP

#include <functional>
#include <memory>
#include <utility>
#include <queue>
#include <stack>

namespace shramko {

template <typename Key, typename Value, typename Compare = std::less<Key>>
class UBstTree {
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using size_type = std::size_t;
    using key_compare = Compare;

private:
    struct Node {
        value_type data;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;

        explicit Node(const value_type& val) : data(val) {}
        explicit Node(value_type&& val) : data(std::move(val)) {}
    };

    Node* root_ = nullptr;
    key_compare comp_{};
    size_type size_ = 0;

    Node* findNode(Node* node, const Key& key) const {
        if (node == nullptr) return nullptr;
        if (comp_(key, node->data.first)) {
            return findNode(node->left, key);
        } else if (comp_(node->data.first, key)) {
            return findNode(node->right, key);
        }
        return node;
    }

    Node* insertNode(Node* node, Node* parent, value_type&& val) {
        if (node == nullptr) {
            Node* newNode = new Node(std::move(val));
            newNode->parent = parent;
            ++size_;
            return newNode;
        }
        if (comp_(val.first, node->data.first)) {
            node->left = insertNode(node->left, node, std::move(val));
        } else if (comp_(node->data.first, val.first)) {
            node->right = insertNode(node->right, node, std::move(val));
        } else {
            node->data.second = std::move(val.second);
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
            minRight->data = std::move(node->data);
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
        return node->data.second;
    }

    const Value& at(const Key& key) const {
        Node* node = findNode(root_, key);
        if (node == nullptr) {
            throw std::out_of_range("Key not found");
        }
        return node->data.second;
    }

    void insert(const value_type& val) {
        root_ = insertNode(root_, nullptr, val);
    }

    void insert(value_type&& val) {
        root_ = insertNode(root_, nullptr, std::move(val));
    }

    void erase(const Key& key) {
        Node* node = findNode(root_, key);
        if (node != nullptr) {
            deleteNode(node);
        }
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
            f(current->data);
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
            f(current->data);
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
            f(current->data);
            if (current->left) q.push(current->left);
            if (current->right) q.push(current->right);
        }
        return f;
    }

private:
    Node* copyTree(Node* node) const {
        if (node == nullptr) return nullptr;
        Node* newNode = new Node(node->data);
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
};

}

#endif
