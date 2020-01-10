#pragma once

#include "types.h"

namespace ATADisk
{
    constexpr size_t SECTOR_SIZE_BYTES = 512;
    constexpr size_t SECTOR_SIZE_WORDS = SECTOR_SIZE_BYTES/2;

    enum class DriveType
    {
        Primary,
        Secondary
    };

    void initialize();

    void read_sectors(u32 start_sector, u16 num_sectors, DriveType drive_type, u8* buff);
    void write_sectors(u32 start_sector, u16 num_sectors, DriveType drive_type, const u8* buff);
}