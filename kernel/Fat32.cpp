#include "Fat32.h"
#include "drivers/ATADisk.h"
#include "types.h"
#include "logging.h"
#include "asserts.h"

// #define FAT32_DBG

static Fat32* s_the = nullptr;

void Fat32::initialize()
{
    ASSERT(s_the == nullptr);
    u8 buff[ATADisk::SECTOR_SIZE_BYTES] = {0};
    ATADisk::read_sectors(0, 1, ATADisk::DriveType::Primary, buff);
    FatBootSector* boot_sector = (FatBootSector*) buff;
    Fat32Extension* extension = (Fat32Extension*) boot_sector->extended_section;
    s_the = new Fat32(*boot_sector, *extension);

}

Fat32& Fat32::the(){
    ASSERT(s_the != nullptr);
    return *s_the;
}

Fat32::Fat32(FatBootSector& boot_sector, const Fat32Extension& extension)
{

    ASSERT(boot_sector.table_count == 2);
    ASSERT(boot_sector.bytes_per_sector == 512);
    ASSERT(boot_sector.reserved_sector_count == 32);
    ASSERT(extension.root_cluster == 2);

    bytes_per_sector = boot_sector.bytes_per_sector;
    FAT_sector = boot_sector.reserved_sector_count;
    FAT_size_in_sectors = extension.table_size_32;
    data_start_sector = FAT_sector + FAT_size_in_sectors*2;
    sectors_per_cluster = boot_sector.sectors_per_cluster;
    root_cluster = extension.root_cluster;

    #ifdef FAT32_DBG
    kprintf("OEM: %s\n", boot_sector.oem_name);
    kprintf("bytes per sector: 0x%x\n", (boot_sector.bytes_per_sector & 0xffff));
    kprintf("reserved sector count: %d\n", boot_sector.reserved_sector_count);
    kprintf("FAT sector: %d\n", FAT_sector);
    kprintf("FAT size in sectors: %d\n", FAT_size_in_sectors);
    kprintf("data start sector: %d\n", data_start_sector);
    kprintf("sectors per cluster: %d\n", sectors_per_cluster);
    kprintf("root cluster: %d\n", root_cluster);

    auto entries = read_directory(root_cluster);
    kprintf("# root dir entries: %d\n", entries.size());
    for(auto& entry : entries)
    {
        if(entry.is_directory())
        {
            continue;
        }
        kprintf("file: %s\n", entry.name);
        auto content = read_whole_entry(entry);
        kprintf("content: %s\n", content->data());
    }
    auto content = read_file(Path("/a/myfile.txt"));
    ASSERT(content.get() != nullptr);
    kprintf("content: %s\n", content->data());
    auto content2 = read_file(Path("/a/subdir/another.dat"));
    ASSERT(content2.get() != nullptr);
    kprintf("content: %s\n", content2->data());
    auto content3 = read_file(Path("/dir2/aa.txt"));
    ASSERT(content3.get() != nullptr);
    kprintf("content: %s\n", content3->data());
    #endif
}

u32 Fat32::cluster_to_sector(u32 cluster) const
{
    return data_start_sector + (cluster-2)*sectors_per_cluster;
}

u32 Fat32::entry_in_fat(u32 cluster) const
{
    u8 buff[ATADisk::SECTOR_SIZE_BYTES] = {0};
    u32 sector_idx = cluster / (bytes_per_sector / 4);
    u32 entry_idx_in_sector = cluster % (bytes_per_sector / 4);
    ATADisk::read_sectors(FAT_sector + sector_idx, 1, ATADisk::DriveType::Primary, buff);
    u32* entries = (u32*) buff;
    u32 val = entries[entry_idx_in_sector];
    // kprintf("FAT:\n");
    // print_hexdump(buff, ATADisk::SECTOR_SIZE_BYTES);
    return val;
}

shared_ptr<Vector<u8>> Fat32::read_cluster(u32 cluster) const
{
    #ifdef FAT32_DBG
    kprintf("read cluster: %d\n", cluster);
    #endif
    auto data_vec = shared_ptr<Vector<u8>>(new Vector<u8>(sectors_per_cluster * bytes_per_sector));
    read_cluster(cluster, data_vec->data());
    data_vec->set_size(sectors_per_cluster * bytes_per_sector);
    return data_vec;
}

void Fat32::read_cluster(u32 cluster, u8* buff) const
{
    u32 start_sector = cluster_to_sector(cluster);
    ATADisk::read_sectors(start_sector, sectors_per_cluster, ATADisk::DriveType::Primary, buff);
}

constexpr u32 FAT_ENTRY_EOF = 0x0FFFFFFF;

Vector<FatDirectoryEntry> Fat32::read_directory(u32 cluster) const
{
    Vector<FatDirectoryEntry> entries;
    u32 current_cluster = cluster;
    bool done = false;
    while(!done)
    {
        auto cluster_data = read_cluster(current_cluster);
        // print_hexdump(cluster_data->data(), cluster_data->size());
        FatDirectoryEntry* current = (FatDirectoryEntry*) cluster_data->data();
        while(!done)
        {
            // skip long names for now
            if(current->is_long_name())
            {
                current++;
                continue;
            }
            // TODO: handle "free" directory entry (name[0] == 0xE5)
            if(current->name[0] == 0){
                done = true;
                break;
            }
            entries.append(*current);
            current++;
        }
        if(done)
            break;
        u32 cluster_entry = entry_in_fat(current_cluster);
        if(cluster_entry == FAT_ENTRY_EOF)
        {
            done = true;
            break;
        }
        u32 next_cluster = (cluster_entry & 0x0fffffff); // take lower 28 bits
        current_cluster = next_cluster;
    }
    return entries;
}


shared_ptr<Vector<u8>> Fat32::read_whole_entry(u32 start_cluster, u32 size) const
{
    // round size up to sector size
    u32 cluster_size = bytes_per_sector * sectors_per_cluster;
    auto data_vec = shared_ptr<Vector<u8>>(new Vector<u8>(size + (cluster_size-(size%cluster_size))));
    u32 current_cluster = start_cluster;
    for(size_t cluster_i = 0; ; cluster_i++)
    {
        read_cluster(current_cluster, data_vec->data() + cluster_size * cluster_i);
        u32 next_cluster = entry_in_fat(current_cluster);
        if(next_cluster == FAT_ENTRY_EOF)
        {
            break;
        }
        current_cluster = (next_cluster & 0x0fffffff); // take lower 28 bits
    }
    return data_vec;
}

shared_ptr<Vector<u8>> Fat32::read_whole_entry(const FatDirectoryEntry& entry) const
{
    return read_whole_entry(entry.cluster_idx(), entry.FileSize);
}

bool Fat32::find_file(const Path& path, FatDirectoryEntry& res) const
{
    #ifdef FAT32_DBG
    kprintf("Fat32: find_file: %s\n", path.to_string().c_str());
    #endif
    ASSERT(path.type() == Path::PathType::Absolute);
    u32 dir_cluster = root_cluster;
    for(size_t i = 0; i < path.num_parts(); ++i)
    {
        auto entries = read_directory(dir_cluster);
        auto part = path.get_part(i);
        bool found = false;
        for(auto& entry : entries)
        {
            if(entry.name_lower() == part)
            {
                if(entry.is_directory())
                {
                    ASSERT(i != path.num_parts() -1);
                    dir_cluster = entry.cluster_idx();
                    found = true;
                    break;
                } else {
                    ASSERT(i == path.num_parts() -1);
                    res = entry;
                    return true;
                }
            }
        }
        if(!found)
            return false;
    }
    return false;
}

shared_ptr<Vector<u8>> Fat32::read_file(const Path& path) const
{
    FatDirectoryEntry res;
    bool found = find_file(path, res);
    if(!found)
    {
        return shared_ptr<Vector<u8>>(nullptr);
    }
    return read_whole_entry(res);
}