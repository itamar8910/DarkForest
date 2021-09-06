#pragma once

#include "types.h"
#include "types/vector.h"
#include "types/String.h"
#include "shared_ptr.h"
#include "FileSystem/path.h"
#include "FileSystem/CharFileSystem.h"
#include "constants.h"
#include "bits.h"
#include "BigBuffer.h"

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

enum class FatDirAttr : u8
{
    ATTR_READ_ONLY = 0x1,
    ATTR_HIDDEN = 0x2,
    ATTR_SYSTEM = 0x4, 
    ATTR_VOLUME_ID = 0x8,
    ATTR_DIRECTORY = 0x10, 
    ATTR_ARCHIVE = 0x20,
    ATTR_LONG_NAME = ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID,
};

struct [[gnu::packed]] FatRawDirectoryEntry
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

    u32 cluster_idx() const;
    String name_lower() const;
    bool is_long_name() const;
    bool is_directory() const;
    bool is_fake_direcotry() const;
    DirectoryEntry::Type get_dir_entry_type() const;

};
static_assert(sizeof(FatRawDirectoryEntry)==32);

struct [[gnu::packed]] FatLongDirectoryEntry
{
    static constexpr u8 LAST_LONG_ENTRY = 0x40;
    static constexpr u8 NAME_LEN_IN_ENTRY = 13;
    static constexpr size_t NAME1_NUM_CHARS = 5;
    static constexpr size_t NAME2_NUM_CHARS = 6;
    static constexpr size_t NAME3_NUM_CHARS = 2;

    u8 ord;
    char name1[NAME1_NUM_CHARS*2];
    u8 attrs;
    u8 type;
    u8 chksum;
    char name2[NAME2_NUM_CHARS*2];
    u16 _unused;
    char name3[NAME3_NUM_CHARS*2];

    String get_name() const;
    bool is_last() const;



};
static_assert(sizeof(FatLongDirectoryEntry)==32);

struct FatDirectoryEntry
{
    String name;
    DirectoryEntry::Type type;
    size_t size;
    u32 cluster_idx; 

    CharDirectoryEntry to_char_directory_entry(const Path& path) const;
    explicit FatDirectoryEntry(const CharDirectoryEntry& cde);
    FatDirectoryEntry();
    explicit FatDirectoryEntry(String name, DirectoryEntry::Type type, size_t size, u32 cluster_idx);
};

class Fat32FS : public CharFileSystem
{
public:
    static void initialize();
    static Fat32FS& the();

    File* open(const Path& path) override;
    virtual bool list_directory(const Path& path, Vector<DirectoryEntry>& res) override;

    virtual bool is_directory(const Path& path) override;
    virtual bool is_file(const Path& path) override;

    bool read_file(const Path& path, u8* data, u32& size) const;
    int write_file(const Path& path, const Vector<u8>& data);

    virtual bool read_file(CharDirectoryEntry& entry, u8* data) const override;
    virtual int write_file(CharDirectoryEntry& entry, const Vector<u8>& data) override;

    virtual bool create_entry(const Path& path, DirectoryEntry::Type type) override;

    virtual u32 cluster_size() const override;
    

private:
    Fat32FS(FatBootSector& boot_sector, const Fat32Extension& extension);
    Vector<FatDirectoryEntry> read_directory(u32 cluster) const;

    u32 cluster_to_sector(u32 cluster) const;
    u32 entry_in_fat(u32 cluster) const;

    void read_cluster(u32 cluster, u8* data) const;

    void write_cluster(u32 cluster, u8* buff) const;

    bool read_whole_entry(u32 start_cluster, u8* data) const;
    bool read_whole_entry(const FatDirectoryEntry& entry, u8* data) const;
    shared_ptr<BigBuffer> read_whole_entry(u32 start_cluster) const;
    size_t num_clusters_in_entry(u32 start_cluster) const;

    int write_to_existing_file(FatDirectoryEntry& entry, const Vector<u8>& data, u32 parent_dir_cluster);

    bool find(const Path& path, FatDirectoryEntry& res, DirectoryEntry::Type type) const;
    bool find_file(const Path& path, FatDirectoryEntry& res) const;
    bool find_directory(const Path& path, FatDirectoryEntry& res) const;

    FatDirectoryEntry create_entry_from(u8* buff, const FatRawDirectoryEntry* raw_entry) const;

    u32 find_free_cluster() const;
    void update_FAT(u32 cluster_idx, u32 value);

    bool update_file_size(u32 dir_cluster, const FatDirectoryEntry& file, size_t size);

    template<typename F>
    bool find_in_diretory(u32 dir_cluster, F lambda, u32& res_cluster, u32& res_cluster_offset);

    static constexpr u32 FAT_ENTRY_EOF = 0xFFFFFF8;

    u32 FAT_sector {0}; // the sector in which the FAT resides
    u32 FAT_size_in_sectors {0};
    u32 data_start_sector {0};
    u32 sectors_per_cluster {0};
    u32 bytes_per_sector {0};
    u32 root_cluster {0};

    // struct CacheEntry
    // {
    //     u32 start_cluster_index {0};
        
    // }
    // Vector<
};

template<typename F>
bool Fat32FS::find_in_diretory(u32 dir_cluster, F lambda, u32& res_cluster, u32& res_cluster_offset)
{
    u32 current_cluster = dir_cluster;
    while(true)
    {
        shared_ptr<BigBuffer> buff = BigBuffer::allocate(cluster_size());
        read_cluster(current_cluster, buff->data());
        FatRawDirectoryEntry* current = reinterpret_cast<FatRawDirectoryEntry*>(buff->data());
        while(
            reinterpret_cast<u8*>(current) < (reinterpret_cast<u8*>(buff->data()) + buff->size())
        )
        {
            if(lambda(*current) == true)
            {
                res_cluster = current_cluster;
                res_cluster_offset = reinterpret_cast<u32>(current) - reinterpret_cast<u32>(buff->data());
                return true;
            }
            current++;
        }

        u32 next_cluster = entry_in_fat(current_cluster);
        if(next_cluster >= FAT_ENTRY_EOF)
        {
            return false;
        }
        current_cluster = (next_cluster & 0x0fffffff); // take lower 28 bits
    }
}
