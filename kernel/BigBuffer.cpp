#include "BigBuffer.hpp"
#include "MM/MemoryManager.h"
#include "Math.h"
#include "bits.h"

shared_ptr<BigBuffer> BigBuffer::allocate(u32 size)
{
    kprintf("BigBuffer::allocate %d\n", size);
    size_t n_pages_required = Math::div_ceil(size, PAGE_SIZE);
    size_t total_pages = (ADDR_END-ADDR_START)/PAGE_SIZE;
    u32* bitmap = get_bitmap();
    for(size_t i = 0; i < total_pages - n_pages_required; ++i)
    {
        bool found = true;
        for(size_t j = i; j < i + n_pages_required; ++j)
        {
            
            bool page_j_free = get_bit(bitmap[j/32], j%32) == 0;
            if(page_j_free == false)
            {
                found = false;
                break;
            }
        }
        if(found)
        {
            for(size_t j = i; j < i + n_pages_required; ++j)
            {
                set_bit(bitmap[j/32], j%32, 1);
                MemoryManager::the().allocate(ADDR_START + (j*PAGE_SIZE), PageWritable::YES, UserAllowed::NO);
            }
            return shared_ptr<BigBuffer>(new BigBuffer(
                size,
                ADDR_START + (i*PAGE_SIZE),
                ADDR_START + ((i+n_pages_required-1)*PAGE_SIZE)
            ));
        }
    }
    kprintf("couldn't allocate BigBuffer with size: %d\n", size);
    return nullptr;
}

u32* BigBuffer::get_bitmap()
{
    static u32 bitmap[(ADDR_END-ADDR_START)/PAGE_SIZE/32] = {0};
    return bitmap;
}

BigBuffer::BigBuffer(u32 size, u32 first_page, u32 last_page)
    : m_size(size),
      m_first_page(first_page),
      m_last_page(last_page)
      {}

BigBuffer::~BigBuffer(){
    u32* bitmap = get_bitmap();
    for(u32 page = m_first_page; page <= m_last_page; page += PAGE_SIZE)
    {
        int page_idx = (page-ADDR_START) / PAGE_SIZE;
        set_bit(bitmap[page_idx/32], page_idx%32, 0);
        MemoryManager::the().deallocate(page);
    }

}

u8* BigBuffer::data()
{
    return reinterpret_cast<u8*>(m_first_page);
}
const u8* BigBuffer::data() const
{
    return reinterpret_cast<const u8*>(m_first_page);
}