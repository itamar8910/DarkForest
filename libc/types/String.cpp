#include "types/String.h"
#include "cstring.h"
#include "asserts.h"
#include "logging.h"

// String::String(): m_chars(nullptr), m_len(0) {
// }
String::String(): m_chars(new char[1]), m_len(0) {
    m_chars[0] = '\0';
}

String::String(const char* str) {
    init_from(str, strlen(str));
}

String::String(const char* str, size_t len) {
    init_from(str, len);
}

String::String(char c) {
    char* tmp = new char[2];
    tmp[0] = c;
    tmp[1] = 0;
    init_from(tmp, 1);
    delete[] tmp;
}

void String::init_from(const char* str, size_t len) {
    #ifdef ASSERTS_LEVEL_1
    ASSERT(str != nullptr);
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
    ASSERT(idx < len());
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
    ASSERT(m_chars != nullptr);
    return m_chars;
}

size_t String::len() const {return m_len;}

String::~String() {
    // printf("a\n");
    if(m_chars != nullptr) {
        delete[] m_chars;
    }
    m_chars = nullptr;
    m_len = 0;
    // printf("after String::dtor");
}


bool String::startswith(const String& other) const {
    // test generate segfault
    if(len() < other.len())
        return false;
    return !strncmp(c_str(), other.c_str(), other.len());
}

String String::substr(int start, int end) const {
    if((end == -1) || (static_cast<size_t>(end)>len())) {
        end = len();
    }
    ASSERT(start>=0);
    ASSERT(end > start);
    // String ctor copies data
    return String(c_str() + start, end-start);
}

int String::find(const String& pattern, size_t start) const {
    auto this_cstr = c_str();
    auto pattern_cstr = pattern.c_str();
    size_t pattern_len = pattern.len();
    for(size_t i = start; i < len(); ++i) {
        if(!strncmp(this_cstr + i, pattern_cstr, pattern_len)) {
            return i;
        }
    }
    return -1;
}

Vector<String> String::split(char delim, size_t capacity) const {
    Vector<String> ret(capacity);
    if(delim == '\0')
        return ret;
    String delim_str(delim);
    size_t prev_idx = 0;
    while(true) {
        int idx = find(delim_str, prev_idx);
        if(idx == -1) {
            if(len() > prev_idx)
            {
                ret.append(substr(prev_idx));
            }
            break;
        }
        ret.append(substr(prev_idx, idx));
        prev_idx = idx+1;
        // skip duplicate delimiters
        while(m_chars[prev_idx] == delim)
        {
            prev_idx++;
        }
    }
    return ret;
}

int String::find_last_of(char c) const
{
    for (int i = len() - 1; i >= 0; --i)
    {
        if (m_chars[i] == c)
        {
            return i;
        }
    }

    return -1;
}

String String::operator+(const String& other) const {
    Vector<char> ret(len() + other.len() + 1);
    ret.concat(m_chars, len());
    ret.concat(other.m_chars, other.len());
    ASSERT(ret.size() == len() + other.len());
    auto res = String(ret.data(), ret.size());
    // ASSERT(res[0] == (*this)[0]);
    // ASSERT(res[len()] == other[0]);
    // ASSERT(res[len()] == other[0]);
    return res;
}

String String::lower() const
{
    Vector<char> ret(len());
    for(size_t i = 0; i < len(); ++i)
    {
        char c = m_chars[i];
        if((c >= 'A' && c <= 'Z'))
            ret.append(c + ('a'-'A'));
        else
            ret.append(c);
    }
    return String(ret.data(), ret.size());
}
