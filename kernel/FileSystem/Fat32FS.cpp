#include "FileSystem/Fat32FS.h"
#include "drivers/ATADisk.h"
#include "types.h"
#include "logging.h"
#include "asserts.h"
#include "CharFile.h"
#include "errs.h"
#include "libc/Math.h"

// #define FAT32_DBG

static Fat32FS* s_the = nullptr;

void Fat32FS::initialize()
{
    ASSERT(s_the == nullptr);
    u8 buff[ATADisk::SECTOR_SIZE_BYTES] = {0};
    ATADisk::read_sectors(0, 1, ATADisk::DriveType::Primary, buff);
    FatBootSector* boot_sector = (FatBootSector*) buff;
    Fat32Extension* extension = (Fat32Extension*) boot_sector->extended_section;
    s_the = new Fat32FS(*boot_sector, *extension);

    // test file creation
    // the().create_file(Path("/rootfile2.txt"));
    // kprintf("added file\n");
    // Vector<DirectoryEntry> entries;
    // the().list_directory(Path("/c"), entries);
    // for(auto& e  : entries)
    // {
    //     kprintf("%s\n", e.path().to_string().c_str());
    // }

}

Fat32FS& Fat32FS::the(){
    ASSERT(s_the != nullptr);
    return *s_the;
}

Fat32FS::Fat32FS(FatBootSector& boot_sector, const Fat32Extension& extension)
    : CharFileSystem(Path("/root"))
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

    kprintf("data start sector: %d\n", data_start_sector);
    kprintf("FAT size in sectors: %d\n", FAT_size_in_sectors);

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

    auto content4 = read_file(Path("a.txt"));
    ASSERT(content4.get() != nullptr);
    kprintf("content: %s\n", content4->data());
    kprintf("content size: %d\n", content4->size());

    open(Path("a.txt"));
    #endif
}

u32 Fat32FS::cluster_to_sector(u32 cluster) const
{
    return data_start_sector + (cluster-2)*sectors_per_cluster;
}

u32 Fat32FS::entry_in_fat(u32 cluster) const
{
    u8 buff[ATADisk::SECTOR_SIZE_BYTES] = {0};
    u32 sector_idx = cluster / (bytes_per_sector / sizeof(u32));
    ASSERT(sector_idx < FAT_size_in_sectors);

    ATADisk::read_sectors(FAT_sector + sector_idx, 1, ATADisk::DriveType::Primary, buff);

    u32* entries = (u32*) buff;
    u32 entry_idx_in_sector = cluster % (bytes_per_sector / sizeof(u32));
    u32 val = entries[entry_idx_in_sector];
    return val;
}

void Fat32FS::read_cluster(u32 cluster, u8* buff) const
{
    u32 start_sector = cluster_to_sector(cluster);
    ATADisk::read_sectors(start_sector, sectors_per_cluster, ATADisk::DriveType::Primary, buff);
}

void Fat32FS::write_cluster(u32 cluster, u8* buff) const
{
    u32 start_sector = cluster_to_sector(cluster);
    ATADisk::write_sectors(start_sector, sectors_per_cluster, ATADisk::DriveType::Primary, buff);
}


Vector<FatDirectoryEntry> Fat32FS::read_directory(u32 cluster) const
{
    kprintf("Read directory: %d\n", cluster);
    Vector<FatDirectoryEntry> entries;
    shared_ptr<BigBuffer> data = read_whole_entry(cluster);
    FatRawDirectoryEntry* current = reinterpret_cast<FatRawDirectoryEntry*>(data->data());
    while(true)
    {
        if(
            reinterpret_cast<u8*>(current) >= (reinterpret_cast<u8*>(data->data()) + data->size())
        )
        {
            // reached end of directory entries without encountering an "end"(=0) directory entry
            // TODO: is this a legal state?
            ASSERT_NOT_REACHED();
        }
        // reached end of directory entries
        if(current->name[0] == 0){
            break;
        }
        if(current->is_long_name())
        {
            // continue to the short directory entries
            // and then traverse back to its (possibly multiple)
            // long directory entries
            current++;
            continue;
        }
        if(current->is_fake_direcotry()){ // skip '.', '..'
            current++;
            continue;
        }
        // TODO: handle "free" directory entry (name[0] == 0xE5)
        constexpr u8 FREE_DIRECTORY_ENTRY = 0xE5;
        if(static_cast<u8>(current->name[0]) == FREE_DIRECTORY_ENTRY)
        {
            current++;
            continue;
        }
        entries.append(create_entry_from(data->data(), current));
        current++;
    }
    return entries;

}


bool Fat32FS::read_whole_entry(u32 start_cluster, u8* data) const
{
    #ifdef FAT32_DBG
    kprintf("read cluster: %d\n", start_cluster);
    #endif
    // kprintf("read whole entry: %d\n", start_cluster);
    // round size up to sector size
    u32 cluster_size = this->cluster_size();
    u32 current_cluster = start_cluster;
    for(size_t cluster_i = 0; ; cluster_i++)
    {
        read_cluster(current_cluster, data + cluster_size * cluster_i);
        u32 next_cluster = entry_in_fat(current_cluster);
        if(next_cluster >= FAT_ENTRY_EOF)
        {
            break;
        }
        current_cluster = (next_cluster & 0x0fffffff); // take lower 28 bits
    }
    return true;
}


shared_ptr<BigBuffer> Fat32FS::read_whole_entry(u32 start_cluster) const
{
    size_t num_clusters = num_clusters_in_entry(start_cluster);
    shared_ptr<BigBuffer> buff = BigBuffer::allocate(num_clusters * cluster_size());

    bool rc = read_whole_entry(start_cluster, buff->data());

    if(rc == false)
        return nullptr;
        
    return buff;
}

size_t Fat32FS::num_clusters_in_entry(u32 start_cluster) const
{
    size_t num_clusters = 1;
    u32 current_cluster = start_cluster;
    for(; ; ++num_clusters)
    {
        u32 next_cluster = entry_in_fat(current_cluster);
        if(next_cluster >= FAT_ENTRY_EOF)
        {
            break;
        }
        current_cluster = (next_cluster & 0x0fffffff); // take lower 28 bits
    }
    return num_clusters;
}

bool Fat32FS::read_whole_entry(const FatDirectoryEntry& entry, u8* data) const
{
    return read_whole_entry(entry.cluster_idx,  data);
}

bool Fat32FS::find(const Path& path, FatDirectoryEntry& res, DirectoryEntry::Type target_type) const
{
    #ifdef FAT32_DBG
    kprintf("Fat32: find: %s\n", path.to_string().c_str());
    #endif

    if(path.num_parts() == 0)
    {
        // return the root directory entry
        res.cluster_idx = root_cluster;
        res.name = "/";
        res.size = 0;
        res.type = DirectoryEntry::Type::Directory;
        return true;
    }

    u32 dir_cluster = root_cluster;
    for(size_t i = 0; i < path.num_parts(); ++i)
    {
        auto entries = read_directory(dir_cluster);
        auto part = path.get_part(i);
        bool found = false;
        for(auto& entry : entries)
        {
            #ifdef FAT32_DBG
            kprintf("trying: %s\n", entry.name_lower().c_str());
            #endif
            if(entry.name == part)
            {
                #ifdef FAT32_DBG
                kprintf("match!\n");
                #endif
                if(entry.type == DirectoryEntry::Type::Directory)
                {
                    if((target_type == DirectoryEntry::Type::File) && (i == path.num_parts() -1))
                    {
                        // can't read a directory
                        return false;
                    }
                    dir_cluster = entry.cluster_idx;
                    if((target_type == DirectoryEntry::Type::Directory) && (i == path.num_parts() -1))
                    {
                        res = entry;
                        return true;
                    }
                    found = true;
                    break;
                } else {
                    if( (target_type == DirectoryEntry::Type::Directory))
                    {
                        // expected to see only directories in path
                        return false;
                    }
                    else if(target_type == DirectoryEntry::Type::File){
                        if((i != path.num_parts() - 1))
                        {
                            // expected to see file only in last component in path
                            return false;
                        }
                    }

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
bool Fat32FS::find_file(const Path& path, FatDirectoryEntry& res) const
{
    kprintf("Fat32: find_file: %s\n", path.to_string().c_str());
    bool rc = find(path, res, DirectoryEntry::Type::File);
    return rc;
}

bool Fat32FS::find_directory(const Path& path, FatDirectoryEntry& res) const
{
    return find(path, res, DirectoryEntry::Type::Directory);
}

bool Fat32FS::read_file(const Path& path, u8* data, u32& size) const
{
    FatDirectoryEntry res;
    bool found = find_file(path, res);
    if(!found)
    {
        return false;
    }
    size = res.size;
    return read_whole_entry(res, data);
}

int Fat32FS::write_file(const Path& path, const Vector<u8>& data)
{
    kprintf("FAT32::write_file: path: %s\n", path.to_string().c_str());
    FatDirectoryEntry res;
    bool found = find_file(path, res);
    if(!found)
    {
        // TODO: see if parent directory exists
        // if it does, add a file to the directory
        NOT_IMPLEMENTED();
    }
    u32 parent_dir_cluster = root_cluster;
    if(path.num_parts() != 1)
    {
        FatDirectoryEntry parent_dir;
        found = find_directory(path.dirname(), parent_dir);
        ASSERT(found==true);
        parent_dir_cluster = parent_dir.cluster_idx;
    }

    return write_to_existing_file(res, data, parent_dir_cluster);
}

int Fat32FS::write_to_existing_file(FatDirectoryEntry& entry, const Vector<u8>& data, u32 parent_dir_cluster)
{
    if((data.size() % ATADisk::SECTOR_SIZE_BYTES) != 0)
    {
        kprintf("Fat32::write data size is not a multiple of sector size\n");
        return E_INVALID_SIZE;
    }
    u32 current_cluster = entry.cluster_idx;
    const size_t num_clusters = Math::div_ceil(data.size(), (sectors_per_cluster * ATADisk::SECTOR_SIZE_BYTES));
    for(size_t cluster_i = 0; cluster_i < num_clusters; ++cluster_i)
    {
        if(current_cluster >= FAT_ENTRY_EOF)
        {
            // TODO: allocate new clusters in FAT
            NOT_IMPLEMENTED();
        }
        const u32 start_sector = cluster_to_sector(current_cluster);
        const u32 data_offset = cluster_i * sectors_per_cluster * ATADisk::SECTOR_SIZE_BYTES;
        const u32 num_sectors_to_write = Math::min(sectors_per_cluster, (data.size() - data_offset) / ATADisk::SECTOR_SIZE_BYTES);
        kprintf("writing to sector: %d\n", start_sector);
        ATADisk::write_sectors(start_sector, num_sectors_to_write, ATADisk::DriveType::Primary, data.data() + data_offset);

        u32 next_cluster = entry_in_fat(current_cluster);
        current_cluster = (next_cluster & 0x0fffffff); // take lower 28 bits
    }
    bool rc = update_file_size(parent_dir_cluster, entry, data.size());
    ASSERT(rc == true);
    return 0;
}

File* Fat32FS::open(const Path& path) {
    FatDirectoryEntry res;
    bool rc = find_file(path, res);
    if(rc == false)
    {
        return nullptr;
    }
    return new CharFile(
        path,
        Fat32FS::the(),
        res.to_char_directory_entry(path),
        res.size
    );
}

bool Fat32FS::list_directory(const Path& path, Vector<DirectoryEntry>& res)
{
    kprintf("Fat32FS::list_directory %s\n", path.to_string().c_str());
    u32 cluster = 0;
    if((path.num_parts() == 0))
    {
        cluster = root_cluster;
    } else{
        FatDirectoryEntry entry;
        bool found = find_directory(path, entry);
        if(!found)
        {
            return false;
        }
        cluster = entry.cluster_idx;
    }
    auto dir_entries = read_directory(cluster);
    for(auto& e : dir_entries)
    {
        res.append(DirectoryEntry(Path(e.name), e.type));
    }
    return true;
}

bool Fat32FS::does_directory_exist(const Path& path)
{
    kprintf("does_directory_exist: path got %s\n", path.to_string().c_str());

    if (path.num_parts() == 0)
    {
        // The root always exists
        return true;
    }

    FatDirectoryEntry entry;
    return find_directory(path, entry);
}

bool Fat32FS::read_file(CharDirectoryEntry& entry, u8* data) const
{
    return read_whole_entry(entry.cluster_idx(), data);

}

int Fat32FS::write_file(CharDirectoryEntry& entry, const Vector<u8>& data)
{
    kprintf("Fat32FS::write_file\n");
    u32 parent_dir_cluster = root_cluster;
    if(entry.path().num_parts() != 1)
    {
        FatDirectoryEntry parent_dir;
        bool found = find_directory(entry.path().dirname(), parent_dir);
        ASSERT(found==true);
        parent_dir_cluster = parent_dir.cluster_idx;
    }

    auto fat_dirent = FatDirectoryEntry(entry);

    return write_to_existing_file(fat_dirent, data, parent_dir_cluster);
}


String FatLongDirectoryEntry::get_name() const
{
    auto unicode_to_ascii = [](const char* arr, size_t len)
    {
        Vector<char> chars;
        ASSERT(len%2 == 0);
        for(size_t i = 0; i < len; i+=2)
        {
            if(
                ((u8)arr[i] == 0xff)
                && ((u8)arr[i+1] == 0xff)
            ){
                continue;
            }
            ASSERT(get_bit(arr[i], 7)==0); // assert ASCII
            ASSERT(arr[i+1] == 0); // assert ASCII
            chars.append(arr[i]);
        }
        return chars;

    };

    Vector<char> chars = unicode_to_ascii(name1, sizeof(name1))
                            + unicode_to_ascii(name2, sizeof(name2)) 
                            + unicode_to_ascii(name3, sizeof(name3));
    chars.append(0);
    return String(chars.data());
}

String FatRawDirectoryEntry::name_lower() const
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

u32 FatRawDirectoryEntry::cluster_idx() const
{
    return (
            static_cast<u32>(ClusterIdxHigh)<<16) 
            | (static_cast<u32>(ClusterIdxLow) & 0xffff
            );
}

bool FatRawDirectoryEntry::is_long_name() const
{
    return attr == static_cast<u8>(FatDirAttr::ATTR_LONG_NAME);
}

bool FatRawDirectoryEntry::is_directory() const
{
    return attr == static_cast<u8>(FatDirAttr::ATTR_DIRECTORY);
}

bool FatRawDirectoryEntry::is_fake_direcotry() const
{
    return (name_lower() == ".") || (name_lower() == "..");
} 
DirectoryEntry::Type FatRawDirectoryEntry::get_dir_entry_type() const
{
    return is_directory() ? DirectoryEntry::Type::Directory : DirectoryEntry::Type::File;
}

CharDirectoryEntry FatDirectoryEntry::to_char_directory_entry(const Path& path) const
{
    return CharDirectoryEntry(
        path, 
        type,
        size,
        cluster_idx
    );
}

FatDirectoryEntry Fat32FS::create_entry_from(u8* buff, const FatRawDirectoryEntry* raw_entry) const
{
    const FatLongDirectoryEntry* current = reinterpret_cast<const FatLongDirectoryEntry*>(raw_entry - 1);
    String name;
    // loop backwards over all Long Directory Entries
    for(;  ;current -= 1)
    {
        ASSERT((void*)current >= (void*)buff);
        name = name + current->get_name(); 
        if(current->is_last())
            break;
    }
    return FatDirectoryEntry
    {
        name,
        raw_entry->get_dir_entry_type(),
        raw_entry->FileSize,
        raw_entry->cluster_idx()
    };
}

FatDirectoryEntry::FatDirectoryEntry(const CharDirectoryEntry& cde)
: name(cde.path().base_name()),
  type(cde.type()),
  size(cde.file_size()),
  cluster_idx(cde.cluster_idx())
{
}

FatDirectoryEntry::FatDirectoryEntry()
: name(""),
  type(DirectoryEntry::Type::File),
  size(0),
  cluster_idx(0)
  {}

 FatDirectoryEntry::FatDirectoryEntry(String name, DirectoryEntry::Type type, size_t size, u32 cluster_idx)
    : name(name),
      type(type),
      size(size),
      cluster_idx(cluster_idx)
{
}

bool FatLongDirectoryEntry::is_last() const
{
    return (ord & LAST_LONG_ENTRY) != 0;
}

u32 Fat32FS::cluster_size() const
{
    return SECTOR_SIZE_BYTES * sectors_per_cluster;
}

bool Fat32FS::create_entry(const Path& path, DirectoryEntry::Type type)
{
    kprintf("Fat32::create entry: %s\n", path.to_string().c_str());
    Path dirname = path.dirname();
    kprintf("create_file: parent directory: %s", dirname.to_string().c_str());
    FatDirectoryEntry res;
    bool rc = find_directory(dirname, res);
    if(!rc)
    {
        kprintf("create_file: parent directory not found\n");
        return false;
    }
    
    kprintf("directory cluster idx: %d\n", res.cluster_idx);
    
    // find empty spot in directory entries
    u32 cluster_with_free_space = 0;
    u32 offset_in_cluster = 0;
    rc = find_in_diretory(res.cluster_idx, [](FatRawDirectoryEntry& entry){
        return entry.name[0] == 0;
    }, cluster_with_free_space, offset_in_cluster);

    ASSERT(rc == true);


    u32 first_cluster_of_file = find_free_cluster();
    ASSERT(first_cluster_of_file != 0);

    const String basename = path.base_name();
    size_t num_long_entries = Math::div_ceil(basename.len(), FatLongDirectoryEntry::NAME_LEN_IN_ENTRY);
    size_t num_entries_left_in_cluster = (cluster_size() - offset_in_cluster) / sizeof(u32);
    if(num_long_entries + 1 > num_entries_left_in_cluster)
    {
        // TODO: we need to allocate an additional cluster in this case
        NOT_IMPLEMENTED();
    }
    shared_ptr<BigBuffer> buff = BigBuffer::allocate(cluster_size());
    read_cluster(cluster_with_free_space, buff->data());
    for(size_t long_entry_i = 0; long_entry_i < num_long_entries; ++long_entry_i)
    {
        FatLongDirectoryEntry long_entry = {};
        // TODO: extract into method FatLongDirectoryEntry::from_name
        long_entry.ord = num_long_entries - long_entry_i;
        if(long_entry_i == num_long_entries-1)
            long_entry.ord |= FatLongDirectoryEntry::LAST_LONG_ENTRY;
        
        String current_part_str = basename.substr(long_entry_i*FatLongDirectoryEntry::NAME_LEN_IN_ENTRY, (long_entry_i+1)*FatLongDirectoryEntry::NAME_LEN_IN_ENTRY);
        Vector<char> current_part(current_part_str.c_str(), current_part_str.len());
        for(size_t i = current_part_str.len(); i < FatLongDirectoryEntry::NAME_LEN_IN_ENTRY; ++i)
        {
            current_part.append(0);
        }
        ASSERT(current_part.size() == FatLongDirectoryEntry::NAME_LEN_IN_ENTRY);

        ascii_to_unicode(long_entry.name1, current_part.data(), FatLongDirectoryEntry::NAME1_NUM_CHARS); 
        ascii_to_unicode(long_entry.name2, current_part.data()+FatLongDirectoryEntry::NAME1_NUM_CHARS, FatLongDirectoryEntry::NAME2_NUM_CHARS); 
        ascii_to_unicode(long_entry.name3, current_part.data()+FatLongDirectoryEntry::NAME1_NUM_CHARS + FatLongDirectoryEntry::NAME2_NUM_CHARS, FatLongDirectoryEntry::NAME3_NUM_CHARS); 
        long_entry.attrs = static_cast<u8>(FatDirAttr::ATTR_LONG_NAME);
        long_entry.type = 0;
        long_entry.chksum = 0; // TODO
        long_entry._unused = 0;


        memcpy(buff->data() + offset_in_cluster + (num_long_entries-1-long_entry_i)*sizeof(FatLongDirectoryEntry), reinterpret_cast<void*>(&long_entry), sizeof(FatLongDirectoryEntry));
    }
    FatRawDirectoryEntry raw_entry = {};
    memcpy(raw_entry.name, basename.c_str(), Math::min(basename.len(), FatLongDirectoryEntry::NAME_LEN_IN_ENTRY));
    // raw_entry.attr = 0;
    raw_entry.attr = (type==DirectoryEntry::Type::File) ? 0 :  static_cast<u8>(FatDirAttr::ATTR_DIRECTORY);
    raw_entry.NTRes = 0;
    raw_entry.CreateTimeTenth = 0;
    raw_entry.CreateTime = 0;
    raw_entry.CreateDate = 0;
    raw_entry.LastAccessDate = 0;
    raw_entry.ClusterIdxHigh = (first_cluster_of_file >> 16) & 0xffff;
    raw_entry.WriteTime = 0;
    raw_entry.WriteDate = 0;
    raw_entry.ClusterIdxLow = first_cluster_of_file & 0xffff;
    raw_entry.FileSize = 0;

    memcpy(buff->data() + offset_in_cluster + num_long_entries*sizeof(FatLongDirectoryEntry), reinterpret_cast<void*>(&raw_entry), sizeof(FatRawDirectoryEntry));

    // update parent directory
    write_cluster(cluster_with_free_space, buff->data());

    // update FAT
    update_FAT(first_cluster_of_file, FAT_ENTRY_EOF);

    // zero allocated cluster
    shared_ptr<BigBuffer> zero_buff = BigBuffer::allocate(cluster_size());
    memset(zero_buff->data(), 0, zero_buff->size());
    write_cluster(first_cluster_of_file, zero_buff->data());

    return true; 
}

u32 Fat32FS::find_free_cluster() const
{
    u8 buff[ATADisk::SECTOR_SIZE_BYTES] = {0};
    const size_t entries_per_fat_sector = SECTOR_SIZE_BYTES/sizeof(u32);
    for(size_t i = 0; i < FAT_size_in_sectors; ++i)
    {
        ATADisk::read_sectors(FAT_sector + i, 1, ATADisk::DriveType::Primary, buff);
        u32* entries = (u32*) buff;
        for(size_t j = 0; j < entries_per_fat_sector; ++j)
        {
            if(entries[j] == 0)
            {
                return (i*entries_per_fat_sector) + j;
            }
        }

    }
    // we ran out of space in the FAT
    ASSERT_NOT_REACHED();
    return 0;
}

void Fat32FS::update_FAT(u32 cluster_idx, u32 value)
{
    const size_t entries_per_fat_sector = SECTOR_SIZE_BYTES/sizeof(u32);
    size_t fat_sector_i = cluster_idx / entries_per_fat_sector;
    size_t entry_index_in_sector = cluster_idx % entries_per_fat_sector;

    u8 buff[ATADisk::SECTOR_SIZE_BYTES] = {0};
    ATADisk::read_sectors(FAT_sector + fat_sector_i, 1, ATADisk::DriveType::Primary, buff);
    u32* entries = (u32*) buff;

    entries[entry_index_in_sector] = value;
    ATADisk::write_sectors(FAT_sector + fat_sector_i, 1, ATADisk::DriveType::Primary, buff);
}

bool Fat32FS::update_file_size(u32 dir_cluster, const FatDirectoryEntry& file, size_t size)
{
    kprintf("Fat32FS::update_file_size to: %d\n", size);
    u32 match_cluster = 0;
    u32 offset_in_cluster = 0;
    bool rc = find_in_diretory(dir_cluster, [&file](FatRawDirectoryEntry& entry){
        return file.cluster_idx == entry.cluster_idx();
    }, match_cluster, offset_in_cluster);
    if(rc == false)
    {
        kprintf("Fat32FS::update_file_size didn't find file in the given directory\n");
        return false;
    }

    shared_ptr<BigBuffer> cluster_data = BigBuffer::allocate(cluster_size());
    read_cluster(match_cluster, cluster_data->data());

    FatRawDirectoryEntry* entry = reinterpret_cast<FatRawDirectoryEntry*>(cluster_data->data() + offset_in_cluster);
    ASSERT(entry->cluster_idx() == file.cluster_idx);

    entry->FileSize = size;

    write_cluster(match_cluster, cluster_data->data());
    return true;
}