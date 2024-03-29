#include "NetworkTypes.h"
#include "stdio_shared.h"

namespace Network
{

String MAC::to_string() const
{
    static constexpr size_t BUFFLEN = 20;
    char buff[BUFFLEN] {};
    size_t current_length = 0;
    for (size_t i =0 ; i < MAC_SIZE; ++i)
    {
        current_length += snprintf(buff+current_length, BUFFLEN - current_length, "%x", data[i]);
        if (i != MAC_SIZE - 1)
        {
            buff[current_length] = ':';
            ++current_length;
        }
    }
    return String(buff, strlen(buff));
}

String IPV4::to_string() const
{
    static constexpr size_t BUFFLEN = 20;
    char buff[BUFFLEN] {};
    snprintf(buff, BUFFLEN, "%d.%d.%d.%d", data[0], data[1], data[2], data[3]);
    return String(buff, strlen(buff));
}

IPV4 IPV4::from_u32(u32 ipv4)
{
    IPV4 ipv4_obj;
    ipv4_obj.data[0] = ipv4>>24;
    ipv4_obj.data[1] = (ipv4>>16) & 0xFF;
    ipv4_obj.data[2] = (ipv4>>8) & 0xFF;
    ipv4_obj.data[3] = (ipv4) & 0xFF;
    return ipv4_obj;
}

u32 IPV4::to_u32()
{
    return data[0]<<24 | data[1] << 16 | data[2] << 8 | data[3];
}

bool IPV4::from_string(String str, IPV4& out)
{
    Vector<char> current_number;
    size_t current_number_index = 0;

    size_t char_index;
    for (char_index = 0; char_index < str.len() && current_number_index < 4; ++char_index)
    {
        if (str[char_index] != '.')
        {
            current_number.append(str[char_index]);
        }
        if (str[char_index] == '.' || char_index == (str.len() - 1)) 
        {
            if (current_number.empty())
            {
                return false;
            }
            current_number.append('\0');
            auto octet = atoi(current_number.data());
            if (octet < 0 || octet > 255)
            {
                return false;
            }
            out.data[current_number_index++] = octet;
            current_number.clear();
            continue;
        }
    }

    return char_index == str.len() && current_number_index == 4;
}

}