#pragma once

#include "types.h"
#include "logging.h"
#include "std_misc.h"
#include "asserts.h"

#define ASSERT_LIST_SIZE

#define MAX_REASONABLE_LIST_SIZE 10000

template<typename T>
class List {

private:
struct Node;

public:


    class Iterator {
    public:
        Iterator(Node* n): current(n) {}
        bool operator==(const Iterator& other) const {
            return this->current == other.current;
        }
        bool operator!=(const Iterator& other) const {
            return !this->operator==(other);
        }
        T& operator*() {return current->val;}
        Iterator& operator++() {
            current = current->next;
            return *this;
        }
        Iterator& operator--() {
            current = current->prev;
            return *this;
        }
        Node* node() {return current;}
    private:
        Node* current; 
    };

    List() 
        : m_head(nullptr),
          m_tail(nullptr),
          m_size(0) {}


    Node* head(){return m_head;}
    Node* tail(){return m_tail;}

    void append(const T& t) {
        #ifdef ASSERT_LIST_SIZE
            if(m_size > MAX_REASONABLE_LIST_SIZE) {
                ASSERT_NOT_REACHED();
            }
        #endif
        append(T(t));
    } 

    void append(T&& t){
        Node* node = new Node(t);
        m_size++;
        if(m_tail == nullptr) {
            m_head = node;
            m_tail = node;
            return;
        }
        m_tail->next = node;
        node->prev = m_tail;
        m_tail = node;
    }

    u32 size() const {return m_size;}

    Iterator find(const T& t) {
        return find([&t](T& other) {return other==t;});
        // for(auto* node = m_head; node != nullptr; node = node->next) {
        //     if(node->val == t) {
        //         return Iterator(node);
        //     }
        // }
        // return end();
    }

    template<typename Func>
    Iterator find(Func match) {
        for(auto* node = m_head; node != nullptr; node = node->next) {
            if(match(node->val)) {
                return Iterator(node);
            }
        }
        return end();
    }

    bool remove(const T& _t) {
        Iterator itr = find(_t);
        if(itr == end()) {
            return false;
        }
        Node* t = itr.node();
        if(t->prev != nullptr) {
            t->prev->next = t->next;
        }
        if(t->next != nullptr) {
            t->next->prev = t->prev;
        }
        if(t == m_head) {
            m_head = t->next;
        }
        if(t == m_tail) {
            m_tail = t->prev;
        }
        delete t;
        m_size--;
        return true;
    }

    Iterator begin() {
        return Iterator(m_head);
    }
    Iterator end() {
        return Iterator(nullptr);
    }

    ~List() {
        Node* cur = m_head;
        while(cur != nullptr) {
            Node* t = cur;
            cur = cur->next;
            delete t;
        }
        m_head = nullptr;
        m_tail = nullptr;
        m_size = 0;
    }


private:

    struct Node {
        T val;
        Node* next;
        Node* prev;
        Node(const T& _val)
            : val(_val),
              next(nullptr),
              prev(nullptr){}

        Node(T&& _val) 
            : val(move(_val)),
              next(nullptr),
              prev(nullptr){}
    };

    Node* m_head;
    Node* m_tail;
    u32 m_size;

};