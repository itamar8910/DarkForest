#include "ATADisk.h"
#include "logging.h"
#include "types.h"
#include "IO.h"
#include "cpu.h"
#include "PIC.h"
#include "asserts.h"
#include "bits.h"
#include "types/vector.h"

namespace ATADisk{
    // static struct ata_device ata_primary_master   = {.io_base = 0x1F0, .control = 0x3F6, .slave = 0};
    enum class DriveType
    {
        Primary,
        Secondary
    };
    constexpr u16 IO_BASE_PRIMARY = 0x1f0;
    constexpr u16 CONTROL_PRIMARY = 0x3F6;
    // constexpr u16 ALTSTATUS = CONTROL;
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

    constexpr u8 SELECT_PRIAMRY = 0xA0;
    constexpr u8 COMMAND_IDENTIFY = 0xEC;

    constexpr u8 STATUS_BIT_ERR = 0;
    constexpr u8 STATUS_BIT_READY = 3;
    constexpr u8 STATUS_BIT_BUSY = 7;

    ISR_HANDLER(ata_primary);
    void isr_ata_primary_handler(RegisterDump& regs) {
        (void)regs;
        kprintf("isr_ata_primary\n");
        PIC::eoi(IRQ_ATA_PRIMARY);
    }

    ISR_HANDLER(ata_secondary);
    void isr_ata_secondary_handler(RegisterDump& regs) {
        (void)regs;
        kprintf("isr_ata_secondary\n");
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

        u8 data_ready = (val & (1<<STATUS_BIT_READY));
        kprintf("data ready: %d\n", data_ready);
    }

    void identify(DriveType type)
    {
        ASSERT(type==DriveType::Primary);

        IO::outb(IO_BASE_PRIMARY + REG_DRIVE_SELECT, SELECT_PRIAMRY);

        IO::outb(IO_BASE_PRIMARY + REG_SECTOR_COUNT, 0);
        IO::outb(IO_BASE_PRIMARY + REG_LBA_LOW, 0);
        IO::outb(IO_BASE_PRIMARY + REG_LBA_MID, 0);
        IO::outb(IO_BASE_PRIMARY + REG_LBA_HIGH, 0);

        IO::outb(IO_BASE_PRIMARY + REG_COMMAND, COMMAND_IDENTIFY);
        u8 val = IO::inb(IO_BASE_PRIMARY + REG_STATUS);
        kprintf("identify status: %d\n", val);
        ASSERT(val != 0);
        poll_until_ready_for_read(type);

        kprintf("data is ready to read\n");
        constexpr size_t IDENTIFY_SIZE = 512;
        uint8_t identify_data[IDENTIFY_SIZE] = {0};
        for(size_t i = 0; i < IDENTIFY_SIZE/2; ++i)
        {
            u16 val = IO::in16(IO_BASE_PRIMARY + REG_DATA);
            identify_data[i*2] = MSB(val);
            identify_data[(i*2)+1] = LSB(val);
        }
        constexpr size_t NAME_OFFSET_START = 27*2;
        constexpr size_t NAME_OFFSET_END = 46*2;
        constexpr size_t NAME_LENGTH = NAME_OFFSET_END-NAME_OFFSET_START+1;
        Vector<char> name(NAME_LENGTH+1);
        memcpy(name.data(), identify_data+NAME_OFFSET_START, NAME_LENGTH);
        name.data()[NAME_LENGTH] = 0;
        kprintf("Device name: %s\n", name.data());
    }

    void initialize()
    {
        kprintf("ATADisk::initialize()\n");
        register_interrupt_handler(IRQ_ISRS_BASE + IRQ_ATA_PRIMARY, isr_ata_primary_entry);
        register_interrupt_handler(IRQ_ISRS_BASE + IRQ_ATA_SECONDARY, isr_ata_secondary_entry);
        identify(DriveType::Primary);
        // // ata_soft_reset(ctrl->dev_ctl);		/* waits until master drive is ready again */
        // u8 slavebit = 0;
        // // identify command
        // IO::outb(IO_BASE + REG_DEVSEL, 0xA0 | slavebit<<4);
        // // wait 400ns
        // IO::inb(CONTROL);
        // IO::inb(CONTROL);
        // IO::inb(CONTROL);
        // IO::inb(CONTROL);
        // unsigned cl=IO::inb(IO_BASE + REG_CYL_LO);	/* get the "signature bytes" */
        // unsigned ch=IO::inb(IO_BASE + REG_CYL_HI);
        // kprintf("cl: 0x%x, ch: 0x%x\n", cl, ch);

    }
}