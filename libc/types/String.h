#pragma once
#include "types/vector.h"
#include "string.h"

class String {

public:
    String();

    String(const char* str);

    void init_from(const char* str, size_t len);

    String(const String& other);

    String(String&& other);

    void operator=(const String& other);
    bool operator!=(const String& other) const;
    bool operator==(const String& other) const;

    size_t len() const;

    const char* c_str() const;

    ~String();

private:
    char* m_chars; // null terminated (for easy convertion to c_str)
    size_t m_len; // not including null terminator
};