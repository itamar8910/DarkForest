#include "ATADisk.h"
#include "logging.h"
#include "types.h"
#include "IO.h"
#include "cpu.h"
#include "PIC.h"
#include "asserts.h"
#include "bits.h"
#include "types/vector.h"
#include "lock.h"

// #define ATA_DISK_DBG

namespace ATADisk{
    // static struct ata_device ata_primary_master   = {.io_base = 0x1F0, .control = 0x3F6, .slave = 0};
    constexpr u16 IO_BASE_PRIMARY = 0x1f0;
    constexpr u16 CONTROL_PRIMARY = 0x3F6;
    constexpr u16 REG_ALT_STATUS_ABS_ADDRESS = CONTROL_PRIMARY;
    constexpr u16 REG_DATA = 0;
    constexpr u16 REG_CYLINDER_LOW = 4;
    constexpr u16 REG_CYLINDER_HIGH = 5;
    constexpr u16 REG_DRIVE_SELECT = 6;
    constexpr u16 REG_SECTOR_COUNT = 2;
    constexpr u16 REG_LBA_LOW = 3;
    constexpr u16 REG_LBA_MID = 4;
    constexpr u16 REG_LBA_HIGH = 5;
    constexpr u16 REG_COMMAND = 7;
    constexpr u16 REG_STATUS = 7;

    constexpr u16 IRQ_ATA_PRIMARY = 14;
    constexpr u16 IRQ_ATA_SECONDARY = 15;

    constexpr u8 IDENTIFY_SELECT_PRIAMRY = 0xA0;
    constexpr u8 COMMAND_IDENTIFY = 0xEC;

    constexpr u8 IO_SELECT_PRIAMRY = 0xE0;
    constexpr u8 COMMAND_READ_SECTORS = 0x20;
    constexpr u8 COMMAND_WRITE_SECTORS = 0x30;
    constexpr u8 COMMAND_CACHE_FLUSH = 0xE7;
    

    constexpr u8 STATUS_BIT_ERR = 0;
    constexpr u8 STATUS_BIT_DRQ = 3;
    constexpr u8 STATUS_BIT_BUSY = 7;
    constexpr u8 STATUS_BIT_DF = 5;
    constexpr u8 STATUS_BIT_RDY = 6;


    ISR_HANDLER(ata_primary);
    void isr_ata_primary_handler(RegisterDump& regs) {
        (void)regs;
        // kprintf("isr_ata_primary\n");
        PIC::eoi(IRQ_ATA_PRIMARY);
    }

    ISR_HANDLER(ata_secondary);
    void isr_ata_secondary_handler(RegisterDump& regs) {
        (void)regs;
        // kprintf("isr_ata_secondary\n");
        PIC::eoi(IRQ_ATA_SECONDARY);
    }

    void poll_until_ready_for_read(DriveType type)
    {
        ASSERT(type==DriveType::Primary);
        constexpr size_t MAX_ATTEMTPS = 100;
        bool breaked = false;
        for(size_t i = 0; i < MAX_ATTEMTPS; ++i)
        {
            u8 val = IO::inb(IO_BASE_PRIMARY + REG_STATUS);
            if((val & (1<<STATUS_BIT_BUSY)) == 0)
            {
                breaked = true;
                break;
            }
            kprintf("tryping to poll again");
        }

        ASSERT(breaked);

        u8 val = IO::inb(IO_BASE_PRIMARY + REG_STATUS);
        ASSERT((val & (1<<STATUS_BIT_ERR)) == 0);

        // u8 data_ready = (val & (1<<STATUS_BIT_DRQ));
    }

    void identify(DriveType type)
    {
        ASSERT(type==DriveType::Primary);

        IO::outb(IO_BASE_PRIMARY + REG_DRIVE_SELECT, IDENTIFY_SELECT_PRIAMRY);

        IO::outb(IO_BASE_PRIMARY + REG_SECTOR_COUNT, 0);
        IO::outb(IO_BASE_PRIMARY + REG_LBA_LOW, 0);
        IO::outb(IO_BASE_PRIMARY + REG_LBA_MID, 0);
        IO::outb(IO_BASE_PRIMARY + REG_LBA_HIGH, 0);

        IO::outb(IO_BASE_PRIMARY + REG_COMMAND, COMMAND_IDENTIFY);
        u8 val = IO::inb(IO_BASE_PRIMARY + REG_STATUS);
        ASSERT(val != 0);
        poll_until_ready_for_read(type);

        constexpr size_t IDENTIFY_SIZE = 512;
        uint8_t identify_data[IDENTIFY_SIZE] = {0};
        for(size_t i = 0; i < IDENTIFY_SIZE/2; ++i)
        {
            u16 val = IO::in16(IO_BASE_PRIMARY + REG_DATA);
            identify_data[i*2] = MSB(val);
            identify_data[(i*2)+1] = LSB(val);
            // kprintf("%d: 0x%x\n", i*2, identify_data[i*2]);
            // kprintf("%d: 0x%x\n", i*2 + 1, identify_data[i*2 + 1]);
        }
        constexpr size_t NAME_OFFSET_START = 27*2;
        constexpr size_t NAME_OFFSET_END = 46*2;
        constexpr size_t NAME_LENGTH = NAME_OFFSET_END-NAME_OFFSET_START+1;
        Vector<char> name(NAME_LENGTH+1);
        memcpy(name.data(), identify_data+NAME_OFFSET_START, NAME_LENGTH);
        name.data()[NAME_LENGTH] = 0;
        kprintf("Device name: %s\n", name.data());

        u8 cylinders = identify_data[1*2+1];
        u8 heads = identify_data[3*2+1];
        u8 sectors_per_track = identify_data[6*2+1];
        kprintf("#cylinders: %d\n", (int) cylinders);
        kprintf("#heads: %d\n", (int)heads);
        kprintf("#sectors/track: %d\n", (int)sectors_per_track);
    }

    void poll(DriveType drive_type)
    {
        ASSERT(drive_type==DriveType::Primary);

        for(int i=0; i< 4; i++)
        {
            IO::inb(REG_ALT_STATUS_ABS_ADDRESS);
            IO::inb(IO_BASE_PRIMARY + 0xc);
        }

        u8 status = 0;
        do
        {
            // kprintf("loop1\n");
            status = IO::inb(IO_BASE_PRIMARY + REG_STATUS);
            // kprintf("status: 0x%d\n", status);
        } while((status & (1<<STATUS_BIT_BUSY)) != 0);

        do
        {
            // kprintf("loop2\n");
            status = IO::inb(IO_BASE_PRIMARY + REG_STATUS);
            // kprintf("status: %d\n", status);
            ASSERT((status & (1<<STATUS_BIT_ERR)) == 0);
            ASSERT((status & (1<<STATUS_BIT_DF)) == 0);
        } while ((status & (1<<STATUS_BIT_DRQ)) == 0);
    }

    void wait_400ns(DriveType drive_type)
    {
        ASSERT(drive_type==DriveType::Primary);
        for(int i=0; i< 4; i++)
        {
            IO::inb(REG_ALT_STATUS_ABS_ADDRESS);
        }
    }

    void select_io_target(u32 start_sector, u16 num_sectors, DriveType drive_type)
    {
        (void) drive_type;
        ASSERT(num_sectors>0);
        u8 type = IO_SELECT_PRIAMRY;

        while((IO::inb(IO_BASE_PRIMARY + REG_STATUS) & (1<<STATUS_BIT_BUSY))!=0){
            // kprintf("loop3\n");
        }

        IO::outb(IO_BASE_PRIMARY + REG_DRIVE_SELECT, type | ((start_sector>>24)& 0x0F));
        IO::outb(IO_BASE_PRIMARY + 1, 0x00);
        IO::outb(IO_BASE_PRIMARY + REG_SECTOR_COUNT, (num_sectors==256) ? 0 : LSB(num_sectors));
        IO::outb(IO_BASE_PRIMARY + REG_LBA_LOW, (u8)(start_sector));
        IO::outb(IO_BASE_PRIMARY + REG_LBA_MID, (u8)(start_sector>>8));
        IO::outb(IO_BASE_PRIMARY + REG_LBA_HIGH, (u8)(start_sector>>16));
    }

    static void sanity_sector_idx(u32 start, u32 count)
    {
        constexpr u32 MAX_SECTOR = 200000;
        if(!(start + count < MAX_SECTOR))
        {
            kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~\nNOTE: this seems to happen sometimes\n"
            "Run ./fs_resetsh and then ./fs_setup.sh");
        }
        ASSERT(start + count < MAX_SECTOR);
    }

    Lock& get_ata_disk_lock()
    {
        static Lock lock("AtaDisk");
        return lock;
    }    

    void read_sectors(u32 start_sector, u16 num_sectors, DriveType drive_type, u8* buff)
    {
        #ifdef ATA_DISK_DBG
        kprintf("read sector: %d\n", start_sector);
        #endif
        LOCKER(get_ata_disk_lock());
        ASSERT(drive_type==DriveType::Primary);
        sanity_sector_idx(start_sector, num_sectors);
        select_io_target(start_sector, num_sectors, drive_type);

        IO::outb(IO_BASE_PRIMARY + REG_COMMAND, COMMAND_READ_SECTORS);

        for(size_t sector_i = 0; sector_i < num_sectors; ++sector_i)
        {
            poll(drive_type);
            for(size_t i = 0; i < SECTOR_SIZE_WORDS; i++)
            {
                u16 val = IO::in16(IO_BASE_PRIMARY + REG_DATA);
                buff[(sector_i*SECTOR_SIZE_BYTES) + i*2] = LSB(val);
                buff[(sector_i*SECTOR_SIZE_BYTES) + i*2 + 1] = MSB(val);
            }
           wait_400ns(drive_type);
        }
    }

    void write_sectors(u32 start_sector, u16 num_sectors, DriveType drive_type, const u8* buff)
    {
        #ifdef ATA_DISK_DBG
        kprintf("write sector: %d\n", start_sector);
        #endif
        LOCKER(get_ata_disk_lock());
        ASSERT(drive_type==DriveType::Primary);
        sanity_sector_idx(start_sector, num_sectors);
        select_io_target(start_sector, num_sectors, drive_type);

        IO::outb(IO_BASE_PRIMARY + REG_COMMAND, COMMAND_WRITE_SECTORS);

        for(size_t sector_i = 0; sector_i < num_sectors; ++sector_i)
        {
            poll(drive_type);
            for(size_t i = 0; i < SECTOR_SIZE_WORDS; i++)
            {
                u16 val = static_cast<u16>(buff[(sector_i*SECTOR_SIZE_BYTES) + i*2]) | (static_cast<u16>(buff[(sector_i*SECTOR_SIZE_BYTES) + i*2 + 1])<<8);
                IO::out16(IO_BASE_PRIMARY + REG_DATA, val, true);
            }
            // wait_400ns(drive_type);
            IO::outb(IO_BASE_PRIMARY + REG_COMMAND, COMMAND_CACHE_FLUSH);
        }
    }

    void initialize()
    {
        kprintf("ATADisk::initialize()\n");
        register_interrupt_handler(IRQ_ISRS_BASE + IRQ_ATA_PRIMARY, isr_ata_primary_entry);
        register_interrupt_handler(IRQ_ISRS_BASE + IRQ_ATA_SECONDARY, isr_ata_secondary_entry);
        PIC::enable_irq(IRQ_ATA_PRIMARY);
        PIC::enable_irq(IRQ_ATA_SECONDARY);
        identify(DriveType::Primary);
        
        // test read/write
        u8 buff[SECTOR_SIZE_BYTES] = {0};
        read_sectors(2, 1, DriveType::Primary, buff);
        u8 orig_value = buff[5];
        buff[5] = 0x1;
        write_sectors(2, 1, DriveType::Primary, buff);
        u8 buff2[SECTOR_SIZE_BYTES] = {0};
        read_sectors(2, 1, DriveType::Primary, buff2);
        ASSERT(buff2[5] == 0x1);
        buff2[5] = orig_value;
        write_sectors(2, 1, DriveType::Primary, buff2);
        buff2[5] = 0;
        read_sectors(2, 1, DriveType::Primary, buff2);
        ASSERT(buff2[5] == orig_value);


    }
}