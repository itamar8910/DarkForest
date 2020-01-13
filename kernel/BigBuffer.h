#pragma once
#include "types.h"
#include "constants.h"
#include "shared_ptr.h"
#include "MM/MemoryManager.h"

class BigBuffer
{
public:
    static shared_ptr<BigBuffer> allocate(u32 size);
    ~BigBuffer();

    u8* data();
    const u8* data() const;

    BigBuffer(const BigBuffer& other) = delete;
    BigBuffer& operator=(const BigBuffer& other) = delete;

private:
    static constexpr u32 ADDR_START = BIG_BUFFER_START;
    static constexpr u32 ADDR_END = BIG_BUFFER_END;
    static u32* get_bitmap();

    BigBuffer(u32 size, u32 first_page, u32 last_page);

    u32 m_size;
    u32 m_first_page;
    u32 m_last_page;
};