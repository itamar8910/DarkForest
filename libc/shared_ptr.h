#pragma once

#include "types.h"
#include "asserts.h"
#include "logging.h"

class Counter {
public:
    Counter(size_t count) : m_count(count){}
    void increment() {++m_count;}
    void decrement(){ASSERT(m_count>=1);--m_count;}
    size_t count() {return m_count;}

private:
    size_t m_count {0};
};

template<typename T>
class shared_ptr 
{
public:
    shared_ptr() 
        : m_counter(nullptr),
          m_instance(nullptr)
          {}
    shared_ptr(T* t)
        : m_counter(new Counter(1)),
          m_instance(t)
          {}
    shared_ptr(const shared_ptr<T>& other) {
        init_from(other);
    }
    shared_ptr<T>& operator=(const shared_ptr<T>& other) {
        if(this==&other)
            return *this;
        init_from(other);
        return *this;
    }
    void init_from(const shared_ptr<T>& other) {
        m_counter = other.m_counter;
        m_instance = other.m_instance;
        if(m_counter) {
            m_counter->increment();
        }
    }

    T* get(){return m_instance;}
    T* operator->(){ASSERT(m_instance); return m_instance;}
    T& operator*() {return *m_instance;}

    size_t num_refs(){
        if(m_counter==nullptr) {
            return 0;
        }
        return m_counter->count();
    }

    void force_free()
    {
        kprintf("WARNING: shared_ptr::force_free\n");
        if(m_counter==nullptr)
            return;
        while(m_counter->count() != 1)
            m_counter->decrement();
        reset();
    }

    void reset() {
        if(m_counter==nullptr)
            return;
        ASSERT(m_counter->count() == 1);
        delete m_counter;
        delete m_instance;
        
    }
    ~shared_ptr() {
        if(m_counter == nullptr) {
            return;
        }
        if(m_counter->count() == 1) {
            reset();
        } else {
            m_counter->decrement();
        }
    }

private:
    Counter* m_counter;
    T* m_instance;
};