#include "string.h"
#include "asserts.h"
#include "FileSystem/TarFS.h"
#include "CharFile.h"
#include "logging.h"


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
    ASSERT(len < 12);
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

File* TarFS::open(const Path& path) {
    auto path_str = path.to_string();
    TarHeader* current = (TarHeader*) base();
    while(strcmp(current->name, "")) {
        size_t size = decode_tarnum(current->size);
        u32 content_addr = (
                ((u32) current) 
                + TAR_PADDING
            );

        if(!strcmp(current->name, path_str.c_str())) {
            shared_ptr<Vector<u8>> data(new Vector<u8>(size));
            memcpy(data->data(), (u8*)content_addr, size);
            return new CharFile(path_str, data, size);
        }
        current = (TarHeader*) round_up(content_addr + size, TAR_PADDING);
    }
    return nullptr;
}

bool TarFS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    (void)path;
    (void)res;
    return false;
}