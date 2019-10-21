#pragma once

#include "types.h"
#include "logging.h"
#include "std_misc.h"

template<typename T>
class List {

private:
struct Node;

public:
    List() 
        : m_head(nullptr),
          m_tail(nullptr),
          m_size(0) {}


    Node* head(){return m_head;}
    Node* tail(){return m_tail;}

    void append(const T& t) {
        append(T(t));
    } 

    void append(T&& t){
        Node* node = new Node(move(t));
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

    Node* find(const T& t) {
        for(auto* node = m_head; node != nullptr; node = node->next) {
            if(node->val == t) {
                return node;
            }
        }
        return nullptr;
    }

    bool remove(const T& _t) {
        Node* t = find(_t);
        if(t == nullptr) {
            return false;
        }
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