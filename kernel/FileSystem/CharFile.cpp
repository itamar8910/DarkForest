#include "CharFile.h"
#include "errs.h"

CharFile::CharFile(const String& path, u8* data, size_t size)
    : File(path),
      m_data(data),
      m_size(size),
      m_idx(0) {}

int CharFile::read(size_t count, void* buf) {
    if(count > (m_size - m_idx)) {
        return E_TOO_BIG;
    }
    memcpy(buf, m_data+m_idx, count);
    m_idx += count;
    return count;
}

int CharFile::write(char* data, size_t count) {
    if(count > (m_size - m_idx)) {
        return E_TOO_BIG;
    }
    memcpy(m_data+m_idx, data, count);
    m_idx += count;	// u8* elf_data = RamDisk::fs().get_content("userspace/HelloWorld.app", elf_size);
	// ASSERT(elf_data != nullptr, "couldn't load HelloWorld");
    return count;
}


char* CharFile::get_content() {
    size_t sz = size();
    char* buff = new char[sz+1];
    read(size(), buff);
    buff[sz] = 0;
    return buff;
}