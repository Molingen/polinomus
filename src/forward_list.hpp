#pragma once
#include <cstddef>
#include <utility>
#include <stdexcept>

template <class T>
class ForwardList {
    struct Node {
        T data;
        Node* next  = nullptr;
        Node()      = default;
        explicit Node(const T& v) : data(v), next(nullptr) {}
        explicit Node(T&& v) : data(std::move(v)), next(nullptr) {}
    };

    Node* first     = nullptr;
    size_t m_size   = 0;

public:
    class Iterator {
        Node* current;

    public:
        explicit Iterator(Node* node)
            : current(node)                          {}
        T& operator*() const                         { return current->data; }
        T* operator->() const                        { return &(current->data); }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        operator bool() const                        { return current != nullptr; }
        bool operator==(const Iterator& other) const { return current == other.current; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
        Node* get_node() const                       { return current; }
    };

    Iterator begin()                                 { return Iterator(first); }
    Iterator end()                                   { return Iterator(nullptr); }
    Iterator before_begin()                          { return Iterator(nullptr); }
    Iterator begin() const                           { return Iterator(first); }
    Iterator end() const                             { return Iterator(nullptr); }
    Iterator before_begin() const                    { return Iterator(nullptr); }

    ForwardList() = default;
    ForwardList(size_t n, const T& d_val) {
        if (n == 0) return;
        first = new Node(d_val);
        Node* curr = first;
        m_size = 1;
        for (size_t i=1; i<n; ++i) {
            Node* tmp = new Node(d_val);
            curr->next = tmp;
            curr = curr->next;
            m_size++;
        }
    }
    ForwardList(const ForwardList& other) {
        if (!other.first) return;
        first = new Node(other.first->data);
        Node* curr = first;
        Node* other_curr = other.first->next;
        m_size = 1;
        while (other_curr) {
            curr->next = new Node(other_curr->data);
            curr = curr->next;
            other_curr = other_curr->next;
            m_size++;
        }
    }
    ForwardList(ForwardList&& other) noexcept
    : first(other.first), m_size(other.m_size) {
        other.first     = nullptr;
        other.m_size    = 0;
    }

    ForwardList& operator=(const ForwardList& other) noexcept {
        if (this != &other) {
            ForwardList temp(other);
            std::swap(*this, temp);
        }
        return *this;
    }
    ForwardList& operator=(ForwardList&& other) noexcept {
        if (this != &other) {
            clear();
            first = other.first;
            m_size = other.m_size;
            other.first = nullptr;
            other.m_size = 0;
        }
        return *this;
    }
    
    ~ForwardList() {
        clear();
    }
    
    void push_front(const T& el) {
        Node* tmp = new Node(el);
        tmp->next = first;
        first = tmp;
        m_size++;
    }
    void push_front(T&& el) {
        Node* tmp = new Node(std::move(el));
        tmp->next = first;
        first = tmp;
        m_size++;
    }
    
    void insert_after(Iterator pos, const T& el) {
        Node* prev = pos.get_node();
        if (!prev) {
            push_front(el);
            return;
        }
        Node* tmp = new Node(el);
        tmp->next = prev->next;
        prev->next = tmp;
        m_size++;
    }

    void erase_front() {
        if (!first) return;
        Node* tmp = first->next;
        delete first;
        first = tmp;
        m_size--;
    }

    void erase_after(Iterator pos) {
        Node* prev = pos.get_node();
        if (!prev) {
            erase_front();
            return;
        }
        if (prev->next) {
            Node* tmp = prev->next->next;
            delete prev->next;
            prev->next = tmp;
            m_size--;
        }
    }
    
    void pop_front() {
        erase_front();
    }
    
    T& operator[](size_t index) {
        Node* curr = first;
        for (size_t i = 0; i < index && curr; i++) {
            curr = curr->next;
        }
        return curr->data;
    }
    
    const T& operator[](size_t index) const {
        Node* curr = first;
        for (size_t i = 0; i < index && curr; i++) {
            curr = curr->next;
        }
        return curr->data;
    }

    T& front() {
        if (!first) throw std::out_of_range("List is empty");
        return first->data;
    }

    const T& front() const {
        if (!first) throw std::out_of_range("List is empty");
        return first->data;
    }
    
    size_t size() const                              { return m_size; }
    bool empty() const                               { return first == nullptr; }

    void clear() {
        Node* curr = first;
        while (curr) {
            Node* tmp = curr->next;
            delete curr;
            curr = tmp;
        }
        first = nullptr;
        m_size = 0;
    }
};
