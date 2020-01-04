#include "CharFile.h"
#include "errs.h"

CharFile::CharFile(const String& path, shared_ptr<Vector<u8>> data, size_t size)
    : File(path),
      m_data(data),
      m_size(size),
      m_idx(0) {}

int CharFile::read(size_t count, void* buf) {
    if(m_data.get() == nullptr)
    {
        return 0;
    }
    if(count > (m_size - m_idx)) {
        return E_TOO_BIG;
    }
    memcpy(buf, m_data->data()+m_idx, count);
    m_idx += count;
    return count;
}

int CharFile::write(char* data, size_t count) {
    if(count > (m_size - m_idx)) {
        return E_TOO_BIG;
    }
    if(m_data.get() == nullptr)
    {
        return E_INVALID;
    }

    memcpy(m_data->data()+m_idx, data, count);
    m_idx += count;	// u8* elf_data = RamDisk::fs().get_content("userspace/HelloWorld.app", elf_size);
	// ASSERT(elf_data != nullptr);
    return count;
}

char* CharFile::get_content() {
    size_t sz = size();
    char* buff = new char[sz+1];
    read(size(), buff);
    buff[sz] = 0;
    return buff;
}