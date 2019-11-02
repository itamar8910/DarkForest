#include "string.h"
#include "Kassert.h"
#include "FileSystem/TarFS.h"


#define TAR_PADDING 512

// Note: all numeric values
// in the tar header are represented as
// a null terminated ascii strings
// where the number is in octal base
struct [[gnu::packed]] TarHeader {
    char name[100];
    char mode[8];
    char uid[8];
    char guid[8];
    char size[12]; 
    char time[12]; // last mod time
    char chksum[8];
    u8 type; // link indicator
};


// decodes number from ascii-string octal representation
static u32 decode_tarnum(char* num) {
    u32 res = 0;
    size_t len = strlen(num);
    ASSERT(len < 12, "tar num string is too long");
    for(size_t i = 0; i < len; i++) {
        res *= 8;
        res += (num[i]-'0');
    }
    return res;
}

// rounds 'num' up so that it would be a multiple of 'round'
u32 round_up(u32 num, u32 round) {
    return num + (round-num)%round;
}

u8* TarFS::get_content(const char* path, u32& ret_size) {
    TarHeader* current = (TarHeader*) m_base;
    while(strcmp(current->name, "")) {
        u32 size = decode_tarnum(current->size);
        u32 content_addr = (
                ((u32) current) 
                + TAR_PADDING
            );

        if(!strcmp(current->name, path)) {
            ret_size = size;
            return (u8*) content_addr;
        }
        current = (TarHeader*) round_up(content_addr + size, TAR_PADDING);
    }
    ret_size = 0;
    return 0;

};