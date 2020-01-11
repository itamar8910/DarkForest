#include "CharFile.h"
#include "errs.h"
#include "constants.h"


int CharFile::read(size_t count, void* buf) {
    kprintf("charfile read: count: %d\n", count);
    // NOTE: we reading the entire file even though
    // the request could be for only a small chunk of it
    auto res = m_fs.read_file(m_dir_entry);
    if(res.get() == nullptr)
    {
        return 0;
    }
    if(count > (m_size - m_idx)) {
        kprintf("too big: count: %d. size: %d, m_idx: %d\n", count, m_size, m_idx);
        return -E_TOO_BIG;
    }
    memcpy(buf, res->data()+m_idx, count);
    m_idx += count;
    return count;
}

int CharFile::write(char* data, size_t count) {
    if(count > (m_size - m_idx)) {
        return E_TOO_BIG;
    }

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