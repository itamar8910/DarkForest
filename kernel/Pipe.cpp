#include "Pipe.h"
#include "asserts.h"
#include "string.h"
#include "Math.h"

int Pipe::read(size_t count, void* buf)
{
    const int num_to_read = Math::min(count, m_data.size() - m_read_idx);
    memcpy(buf, m_data.data() + m_read_idx, num_to_read);
    m_read_idx += num_to_read;
    return num_to_read;
}

int Pipe::write(char* data, size_t count)
{
    m_data.concat(data, count);
    return count;
}


int Pipe::ioctl(u32 , void* )
{
    NOT_IMPLEMENTED();
}

bool Pipe::can_read() const
{
    return m_read_idx < m_data.size();
}
