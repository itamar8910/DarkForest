#pragma once

#include "types.h"
#include "types/vector.h"
#include "types/String.h"
#include "shared_ptr.h"
#include "FileSystem/path.h"
#include "FileSystem/CharFileSystem.h"
#include "constants.h"

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

    u32 cluster_idx() const
    {
        return (
                static_cast<u32>(ClusterIdxHigh)<<16) 
                | (static_cast<u32>(ClusterIdxLow) & 0xffff
                );
    }

    String name_lower() const
    {
        String name_str(name, 11);
        // name[0..7] = base name
        // name[8..10] = extension
        int space_idx = name_str.find(" ");
        String basename = name_str.substr(0, (space_idx == -1) ? 8 : space_idx);
        String extension = name_str.substr(8);
        // String res = basename + String(".") + extension;
        String res = basename;
        if(extension[0] != ' ') // if extension is not empty
        {
            res = res + String(".") + extension;
        }
        res = res.lower();
        return res;
    }

    bool is_long_name() {
        return attr == static_cast<u8>(FatDirAttr::ATTR_LONG_NAME);
    }

    bool is_directory()
    {
        return attr == static_cast<u8>(FatDirAttr::ATTR_DIRECTORY);
    }
    
    bool is_fake_direcotry()
    {
        return (name_lower() == ".") || (name_lower() == "..");
    }

    DirectoryEntry::Type get_dir_entry_type()
    {
        return is_directory() ? DirectoryEntry::Type::Directory : DirectoryEntry::Type::File;
    }

    CharDirectoryEntry to_char_directory_entry(const Path& path)
    {
        return CharDirectoryEntry(
            path, 
            get_dir_entry_type(),
            FileSize,
            cluster_idx()
        );
    }

};
static_assert(sizeof(FatDirectoryEntry)==32);


class Fat32FS : public CharFileSystem
{
public:
    static void initialize();
    static Fat32FS& the();

    File* open(const Path& path) override;
    virtual bool list_directory(const Path& path, Vector<DirectoryEntry>& res) override;

    virtual bool does_directory_exist(const Path& path) override;

    bool read_file(const Path& path, u8* data, u32& size) const;
    int write_file(const Path& path, const Vector<u8>& data);

    // virtual shared_ptr<Vector<u8>> read_file(CharDirectoryEntry& entry) const override;
    virtual bool read_file(CharDirectoryEntry& entry, u8* data) const override;
    virtual int write_file(CharDirectoryEntry& entry, const Vector<u8>& data) override;

    virtual u32 cluster_size() const override
    {
        return SECTOR_SIZE_BYTES * sectors_per_cluster;
    }


private:
    Fat32FS(FatBootSector& boot_sector, const Fat32Extension& extension);
    Vector<FatDirectoryEntry> read_directory(u32 cluster) const;

    u32 cluster_to_sector(u32 cluster) const;
    u32 entry_in_fat(u32 cluster) const;

    void read_cluster(u32 cluster, u8* data) const;

    void write_cluster(u32 cluster, u8* buff) const;

    bool read_whole_entry(u32 start_cluster, u32 size, u8* data) const;

    bool read_whole_entry(const FatDirectoryEntry& entry, u8* data) const;

    int write_to_existing_file(CharDirectoryEntry& entry, const Vector<u8>& data);

    bool find(const Path& path, FatDirectoryEntry& res, DirectoryEntry::Type type) const;
    bool find_file(const Path& path, FatDirectoryEntry& res) const;
    bool find_directory(const Path& path, FatDirectoryEntry& res) const;



    
    u32 FAT_sector {0}; // the sector in which the FAT resides
    u32 FAT_size_in_sectors {0};
    u32 data_start_sector {0};
    u32 sectors_per_cluster {0};
    u32 bytes_per_sector {0};
    u32 root_cluster {0};
};
