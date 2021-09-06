#include "CharFile.h"
#include "errs.h"
#include "constants.h"
#include "BigBuffer.h"
#include "Math.h"
#include "unix.h"

int CharFile::read(size_t count, void* buf) {
    kprintf("file: %s, read count: %d\n", m_dir_entry.path().to_string().c_str(), count);
    if(count == 0)
        return 0;
    // NOTE: we reading the entire file even though
    // the request could be for only a small chunk of it
    // FIXME: m_content_cache is not safe in case multiple File objects write to the same file concurrently
    //        move this cache to the filesystem?
    if (!m_content_cache.get()) {
        m_content_cache = BigBuffer::allocate(Math::round_up(m_size, m_fs.cluster_size()));
        bool rc = m_fs.read_file(m_dir_entry, m_content_cache->data());
        if(rc == false)
        {
            return 0;
        }
    }
    if(count > (m_size - m_idx)) {
        kprintf("CharFile::Read too big: count: %d. size: %d, m_idx: %d\n", count, m_size, m_idx);
        return -E_TOO_BIG;
    }
    memcpy(buf, m_content_cache->data()+m_idx, count);
    m_idx += count;
    return count;
}

int CharFile::write(char* data, size_t count) {
    kprintf("CharFile::write with size: %d\n", count);
    m_content_cache.reset();
    // if(count > (m_size - m_idx)) {
    //     return E_TOO_BIG;
    // }

    Vector<u8> to_write(reinterpret_cast<u8*>(data), count);
    kprintf("to_write size before pad: %d\n", to_write.size());
    // pad to be a multiple of sector size
    size_t leftover = SECTOR_SIZE_BYTES - (count % SECTOR_SIZE_BYTES);
    for(size_t i = 0; i < leftover; ++i)
    {
        to_write.append(0);
    }
    kprintf("writing with size: %d\n", to_write.size());
    m_fs.write_file(m_dir_entry, to_write);

    m_idx += count;

    return count;
}

char* CharFile::get_content() {
    size_t sz = size();
    char* buff = new char[sz+1];
    read(size(), buff);
    buff[sz] = 0;
    return buff;
}

int CharFile::lseek(int offset, int whence) 
{
    kprintf("file: %s, lseek: %d\n", m_dir_entry.path().to_string().c_str(), offset);
    switch (whence) {
        case SEEK_CUR:
            m_idx += offset;
            break;
        case SEEK_SET:
            m_idx = offset;
            break;
        case SEEK_END:
            ASSERT(offset <= 0);
            m_idx = m_size + offset;
            break;
        default:
            ASSERT_NOT_REACHED();
    }
    ASSERT(m_idx <= m_size);
    return m_idx;
}
