#pragma once

#include "types.h"
#include "std_misc.h"
#include "new.h"
#include "string.h"
#include "asserts.h"
#include "stdlib.h"
#include "logging.h"
#ifdef KERNEL
#include "kernel/kmalloc.h"
#endif
#ifdef USERSPACE
#include "malloc.h"
#endif

#define DEFAULT_VECTOR_CAPACITY 8
// #define VECTOR_DBG


template <typename T>
class Vector {
public:


    class Iterator {
    public:
        Iterator(Vector& vector, size_t idx) 
            : m_vector(vector),
              m_idx(idx)
            {}
        Iterator& operator++() {++m_idx; return *this;}
        Iterator& operator--() {--m_idx; return *this;}
        bool operator==(const Iterator& other) {return m_idx == other.m_idx;}
        bool operator!=(const Iterator& other) {return !this->operator==(other);}
        T& operator*() {return m_vector.at(m_idx);}
    private:
        Vector& m_vector;
        size_t m_idx;
    };

    Vector() 
    {
        init_with_capacity(DEFAULT_VECTOR_CAPACITY);
        #ifdef VECTOR_DBG
        kprintf("Vector::ctor\n");
        kprintf("Vector::ctor - data allocated addr: 0x%x\n", m_data);
        #endif
    }

    Vector(size_t init_capacity) {
        init_with_capacity(init_capacity);
    }

    void init_with_capacity(size_t c) {
        if(c==0) {
            c = DEFAULT_VECTOR_CAPACITY;
        }
        m_capacity = c;
        m_size = 0;
        m_data = (T*) allocate(c * sizeof(T));
    }

    Vector(const T* data, size_t size) 
        :
            // m_data(new T[size]),
            m_capacity (size),
            m_size(size) 
    {
        m_data = (T*) allocate(size * sizeof(T));
        memcpy(m_data, data, size);
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
        m_size ++;
    }
    void concat(const T* data, size_t len) {
        ensure_capacity(size() + len);    
        for(size_t i = 0; i < len; i++) {
            new(&m_data[m_size]) T(move(data[i]));
            m_size ++;
        }
    }

    // TODO: impl. rvalue variant
    Vector(const Vector& other) {
        initialize_from(other);
    }

    // TODO: impl. rvalue variant
    Vector& operator=(const Vector& other) {
        if(this != &other) {
            deallocate();
            initialize_from(other);
        }
        return *this;
    }

    T* data() {return m_data;}
    const T* data() const {return m_data;}

    Iterator begin() {return Iterator(*this, 0);}
    Iterator end() {return Iterator(*this, m_size);}

    void clear() {
        #ifdef VECTOR_DBG
        dbgprintf("Vector::clear()\n");
        #endif
        deallocate();
        init_with_capacity(DEFAULT_VECTOR_CAPACITY);

    }


    ~Vector() {
        #ifdef VECTOR_DBG
        dbgprintf("Vector::dtor\n");
        #endif
        deallocate();
    }

private:
    T& access(size_t pos) {
        ASSERT(pos < m_size);
        return m_data[pos];
    }
    const T& access(size_t pos) const {
        ASSERT(pos < m_size);
        return m_data[pos];
    }

    void move_to_buffer(T* new_data) {
        for(size_t i = 0; i < m_size; i++) {
            new(&new_data[i]) T(move(m_data[i]));
            m_data[i].~T();
        }
        #ifdef KERNEL
        kfree(m_data);
        #endif
        #ifdef USERSPACE
        free(m_data);
        #endif
        // delete[] m_data;
        m_data = nullptr;
    }


    /// doubles capacity & re-allocates data buffer
    /// if cap is greater than current capacity
    void ensure_capacity(size_t cap, bool check_capacity=true) {
        #ifdef VECTOR_DBG
        kprintf("Vector::ensure_capacity: %d\n", cap);
        #endif
        if(cap <= m_capacity) {
            return;
        }
        dbgprintf("Vector::ensure_capacity: growing: %d->%d\n", m_capacity, m_capacity*2);
        #ifdef VECTOR_DBG
        kprintf("Vector::ensure_capacity: need to re-allocate\n");
        #endif
        if(check_capacity) {
            ASSERT(m_capacity * 2 >= cap);
        }
        // T* new_data = new T[sizeof(T) * m_capacity * 2];
        T* new_data = (T*) allocate(sizeof(T) * m_capacity * 2);
        #ifdef VECTOR_DBG
        kprintf("Vector::ensure_capacity - new_data size: %d items\n",  m_capacity * 2);
        kprintf("Vector::ensure_capacity - new_data allocated addr: 0x%x\n", new_data);
        #endif
        move_to_buffer(new_data);
        m_data = new_data;
        m_capacity *= 2;
    }

    void deallocate() {
        #ifdef VECTOR_DBG
        dbgprintf("Vector::deallocate()\n");
        #endif
        for(size_t i = 0; i < m_size; i++ ){
            m_data[i].~T();
        }
        deallocate(m_data);
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    void initialize_from(const Vector& other) {
        // m_data = new T[other.m_capacity];
        m_data = (T*) allocate(other.m_capacity * sizeof(T));
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        for(size_t i = 0; i < other.m_size; i++) {
            new(&m_data[i]) T(other.m_data[i]);
        }
    }

    static void* allocate(size_t size) {
        #ifdef KERNEL
        return kmalloc(size);
        #endif
        #ifdef USERSPACE
        return malloc(size);
        #endif
    }

    static void deallocate(void* p) {
        #ifdef KERNEL
        kfree(p);
        #endif
        #ifdef USERSPACE
        free(p);
        #endif

    }

    T* m_data;
    size_t m_capacity;
    size_t m_size;
    
};
