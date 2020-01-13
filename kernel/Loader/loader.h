#pragma once

#include "types.h"
#include "types/String.h"
#include "FileSystem/VFS.h"
#include "shared_ptr.h"
#include "BigBuffer.h"

struct UserspaceLoaderData
{
    char glob_load_path[MAX_PATH_LEN];
    char** argv;
    size_t argc;
};

void load_and_jump_userspace(UserspaceLoaderData&);
void load_and_jump_userspace(const String& path);
void load_and_jump_userspace(shared_ptr<BigBuffer> elf_data_ptr,
                                u32 size,
                                char** argv=nullptr,
                                size_t argc=0);

extern "C" void jump_to_usermode(void (*func)(), u32 user_esp);

void clone_args(char**& argv_dst, size_t& argc_dst, char** argv_src, size_t argc_src);