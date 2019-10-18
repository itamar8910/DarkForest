#pragma once

#include "types.h"
#include "std_misc.h"
#include "new.h"
#include "string.h"
#include "Kassert.h"

#define DEFAULT_VECTOR_CAPACITY 8
// #define VECTOR_DBG

template <typename T>
class Vector {
public:
    Vector() : 
                    m_data(new T[DEFAULT_VECTOR_CAPACITY]),
                    m_capacity (DEFAULT_VECTOR_CAPACITY),
                    m_size(0)
    {
        #ifdef VECTOR_DBG
        kprintf("Vector::ctor\n");
        kprintf("Vector::ctor - data allocated addr: 0x%x\n", m_data);
        #endif
    }
    size_t size() const { return m_size;};
    size_t capacity() const { return m_capacity;};
    T& at(size_t pos) { return access(pos);}
    const T& at(size_t pos) const {return access(pos);}

    /// note: operator[] DOES check bounds
    T& operator[] (size_t pos) {return access(pos);}
    const T& operator[] (size_t pos) const {return access(pos);}

    bool empty() const { return m_size != 0; }

    /// like push_back
    void append(const T& value) {
        append(T(value));
    }
    void append(T&& value) {
        ensure_capacity(size() + 1);
        new(&m_data[m_size]) T(move(value));
        ASSERT(m_data[m_size] == value, "vector::append sanity");
        m_size ++;
    }

    // TODO: impl. rvalue variant
    Vector(const Vector& other) {
        initialize_from(other);
    }

    // TODO: impl. rvalue variant
    Vector& operator=(const Vector& other) {
        if(this != &other) {
            clear();
            initialize_from(other);
        }
        return *this;
    }

    ~Vector() {
        #ifdef VECTOR_DBG
        kprintf("Vector::dtor\n");
        #endif
        clear();
    }

private:
    T& access(size_t pos) {
        ASSERT(pos < m_size, "Vector access of out bounds");
        return m_data[pos];
    }
    const T& access(size_t pos) const {
        ASSERT(pos < m_size, "Vector access of out bounds");
        return m_data[pos];
    }

    void move_to_buffer(T* new_data) {
        for(size_t i = 0; i < m_size; i++) {
            new(&new_data[i]) T(move(m_data[i]));
            m_data[i].~T();
        }
        delete[] m_data;
        m_data = nullptr;
    }

    /// doubles capacity & re-allocates data buffer
    /// if cap is greater than current capacity
    void ensure_capacity(size_t cap) {
        #ifdef VECTOR_DBG
        kprintf("Vector::ensure_capacity: %d\n", cap);
        #endif
        if(cap <= m_capacity) {
            return;
        }
        #ifdef VECTOR_DBG
        kprintf("Vector::ensure_capacity: need to re-allocate\n");
        #endif
        ASSERT(m_capacity * 2 >= cap, "Vector::ensure_capacity - capacity too large");
        T* new_data = new T[sizeof(T) * m_capacity * 2];
        #ifdef VECTOR_DBG
        kprintf("Vector::ensure_capacity - new_data size: %d items\n",  m_capacity * 2);
        kprintf("Vector::ensure_capacity - new_data allocated addr: 0x%x\n", new_data);
        #endif
        move_to_buffer(new_data);
        m_data = new_data;
        m_capacity *= 2;
    }

    void clear() {
        #ifdef VECTOR_DBG
        kprintf("Vector::clear()\n");
        #endif
        for(size_t i = 0; i < m_size; i++ ){
            m_data[i].~T();
        }
        delete[] m_data;
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    void initialize_from(const Vector& other) {
        m_data = new T[other.m_capacity];
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        for(size_t i = 0; i < other.m_size; i++) {
            new(&m_data[i]) T(other.m_data[i]);
        }
    }

    T* m_data;
    size_t m_capacity;
    size_t m_size;
    
};
