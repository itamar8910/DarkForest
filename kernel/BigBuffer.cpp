#include "BigBuffer.h"
#include "MM/MemoryManager.h"
#include "Math.h"
#include "bits.h"
#include "lock.h"

// #define BIGBUFFER_DBG

Lock& big_buffer_lock()
{
    static Lock lock("BigBuffer");
    return lock;
}

shared_ptr<BigBuffer> BigBuffer::allocate(u32 size, bool verify_contiguous)
{
    LOCKER(big_buffer_lock());
    ASSERT(size>0);
#ifdef BIGBUFFER_DBG
    kprintf("BigBuffer::allocate %d\n", size);
#endif
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
#ifdef BIGBUFFER_DBG
            kprintf("BigBuffer: allocated base addr: 0x%x\n", ADDR_START + (i*PAGE_SIZE));
#endif
            auto big_buffer = shared_ptr<BigBuffer>(new BigBuffer(
                size,
                ADDR_START + (i*PAGE_SIZE),
                ADDR_START + ((i+n_pages_required-1)*PAGE_SIZE)
            ));

            if (verify_contiguous) {
                uint32_t current_phsyical_address = 0;
                for (size_t page_i = 0; page_i < n_pages_required; ++page_i)
                {
                    auto virtual_addr = big_buffer->m_first_page + page_i*PAGE_SIZE;
                    auto addr = MemoryManager::the().get_physical_address(virtual_addr);
                    kprintf("verify_contiguous: virtual addr is %p, physical addr is %p\n", virtual_addr, addr);
                    if (!current_phsyical_address) {
                        current_phsyical_address = addr;
                        continue;
                    }
                    // TODO: Instead of "verify_continuous", implement actual "continues physical memory allocation".
                    ASSERT(addr == current_phsyical_address + PAGE_SIZE);
                    current_phsyical_address = addr;
                }
            }
            return big_buffer;
        }
    }
    kprintf("WARNING: Couldn't allocate BigBuffer with size: %d\n", size);
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
    LOCKER(big_buffer_lock());
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

u32 BigBuffer::size() const
{
    return m_size;
}