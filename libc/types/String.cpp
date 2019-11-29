#include "types/String.h"
#include "string.h"
#include "asserts.h"
#include "logging.h"

String::String(): m_chars() {}

String::String(const char* str) {
    #ifdef ASSERTS_LEVEL_1
    ASSERT(str != nullptr, "err constructing string from nullptr char* (1)");
    #endif
    init_from(str, strlen(str));
}

String::String(char c) {
    init_from(&c, 1);
}

void String::init_from(const char* str, size_t len) {
    #ifdef ASSERTS_LEVEL_1
    ASSERT(str != nullptr, "err constructing string from nullptr char* (2)");
    #endif
    m_chars = new char[len+1];
    memcpy(m_chars, str, len);
    m_chars[len] = 0;
    m_len = len;
}

String::String(const String& other) {
    init_from(other.m_chars, other.len());
}

String::String(String&& other) {
    m_chars = other.m_chars;
    m_len = other.len();
    other.m_chars = nullptr;
    other.m_len = 0;
}

char String::operator[](size_t idx) const {
    ASSERT(idx < len(), "String::operator[] idx overflow");
    return c_str()[idx];
}

bool String::operator==(const String& other) const {
    return len() == other.len() 
            && !memcmp(m_chars, other.m_chars, len());
}
bool String::operator!=(const String& other) const {
    return len() != other.len() 
            || memcmp(m_chars, other.m_chars, len());
}

void String::operator=(const String& other) {
    init_from(other.m_chars, other.len());
}

const char* String::c_str() const {
    return m_chars;
}

size_t String::len() const {return m_len;}

String::~String() {
    delete[] m_chars;
    m_len = 0;
}

// #define GENERATE_SEGFAULT

void  __attribute__ ((noinline)) test_generate_segfault() {
    char* p = nullptr;
    *p = 0;
}

bool String::startswith(const String& other) const {
    #ifdef GENERATE_SEGFAULT
    test_generate_segfault();
    #endif
    // test generate segfault
    if(len() < other.len())
        return false;
    return !strncmp(c_str(), other.c_str(), other.len());
}

String String::substr(int start, int end) const {
    if(end == -1) {
        end = len();
    }
    char* buff = new char[end-start];
    memcpy(buff, c_str()+start, end-start);
    return String(buff);
}

int String::find(const String& pattern, size_t start) {
    auto this_cstr = c_str();
    auto pattern_cstr = pattern.c_str();
    for(size_t i = start; i < len(); ++i) {
        if(!strcmp(this_cstr + i, pattern_cstr)) {
            return i;
        }
    }
    return -1;
}

Vector<String> String::split(char delim) {
    printf("String::split\n");
    Vector<String> ret;
    String delim_str(delim);
    int prev_idx = 0;
    while(true) {
        int idx = find(delim_str, prev_idx);
        printf("idx: %d\n", idx);
        if(idx == -1) {
            break;
        }
        ret.append(substr(prev_idx, idx));
        idx = prev_idx+1;
    }
    return ret;
}