#pragma once

#include "types.h"
#include "types/vector.h"
#include "types/String.h"
#include "shared_ptr.h"

struct [[gnu::packed]] Fat32Extension
{
	unsigned int		table_size_32;
	unsigned short		extended_flags;
	unsigned short		fat_version;
	unsigned int		root_cluster;
	unsigned short		fat_info;
	unsigned short		backup_BS_sector;
	unsigned char 		reserved_0[12];
	unsigned char		drive_number;
	unsigned char 		reserved_1;
	unsigned char		boot_signature;
	unsigned int 		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
};
 
struct [[gnu::packed]] FatBootSector
{
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	    bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char		extended_section[54];
 
};

struct [[gnu::packed]] FatDirectoryEntry
{
    char name[11];
    u8 attr;
    u8 NTRes;
    u8 CreateTimeTenth;
    u16 CreateTime;
    u16 CreateDate;
    u16 LastAccessDate;
    u16 ClusterIdxHigh;
    u16 WriteTime;
    u16 WriteDate;
    u16 ClusterIdxLow;
    u32 FileSize;
};
static_assert(sizeof(FatDirectoryEntry)==32);

class Fat32
{
public:
    static void initialize();
    static Fat32& the();

private:
    Fat32(FatBootSector& boot_sector, const Fat32Extension& extension);
    Vector<FatDirectoryEntry> read_directory(u32 cluster) const;

    u32 cluster_to_sector(u32 cluster) const;
    u32 entry_in_fat(u32 cluster) const;
    shared_ptr<Vector<u8>> read_cluster(u32 cluster) const;


    u32 FAT_sector {0};
    u32 FAT_size_in_sectors {0};
    u32 data_start_sector {0};
    u32 sectors_per_cluster {0};
    u32 bytes_per_sector {0};
    u32 root_cluster {0};
};
