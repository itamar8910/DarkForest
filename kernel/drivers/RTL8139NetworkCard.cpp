#include "RTL8139NetworkCard.h"
#include "asserts.h"
#include "PCIBus.h"
#include "IO.h"
#include "logging.h"
#include "MM/MemoryManager.h"
#include "PIC.h"
#include "cpu.h"

#define VENDOR_ID 0x10EC
#define DEVICE_ID 0x8139
#define CONFIG_REG_1 0x52
#define COMMAND_REG 0x37
#define RECEIVEBUFFER_START_REG 0x30
#define IMR_REG 0x3c
#define RCR_REG 0x44

static RTL8139NetworkCard* s_the = nullptr;

RTL8139NetworkCard& RTL8139NetworkCard::the()
{
   ASSERT(s_the != nullptr);
   return *s_the;
}

void RTL8139NetworkCard::initialize()
{
    PCIBus::PciDeviceMetadata device_metadata {};

    if (!PCIBus::get_device_metadata(VENDOR_ID, DEVICE_ID, device_metadata))
    {
        kprintf("ERROR: could not find RTL8139 network card PCI device\n");
        ASSERT_NOT_REACHED();
    }
    s_the = new RTL8139NetworkCard(device_metadata);
}

ISR_HANDLER(rtl8139)
void isr_rtl8139_handler(RegisterDump& regs) {
    (void)regs;
    kprintf("isr_rtl8139_handler interrupt!");
    PIC::eoi(RTL8139NetworkCard::the().irq_number());
    IO::out32(RTL8139NetworkCard::the().io_base_address() + 0x3E, 0x5);
}


RTL8139NetworkCard::RTL8139NetworkCard(PCIBus::PciDeviceMetadata device_metadata)
     : m_device_metadata(device_metadata)
{
   kprintf("RTL8139NetworkCard::ctor()\n");

   m_irq_nuber = device_metadata.irq_number;
   kprintf("irq number: %d\n", device_metadata.irq_number);

   verify_bus_mastering();
   turn_on();
   software_reset();
   init_recv_buffer();
   kprintf("a1\n");
   setup_interrupt_mask();
   kprintf("a2\n");
   configure_receive();
   kprintf("a3\n");
   enable_receive_transmit();
   kprintf("a4\n");
    register_interrupt_handler(IRQ_ISRS_BASE + device_metadata.irq_number, isr_rtl8139_entry);

    PIC::enable_irq(device_metadata.irq_number);


}

void RTL8139NetworkCard::enable_receive_transmit()
{
    IO::outb(m_device_metadata.io_base_address + COMMAND_REG, 0x0C);
}

void RTL8139NetworkCard::configure_receive()
{
    uint32_t RULES = 0xf; // AB+AM+APM+AAP
    uint32_t NO_WRAP = (1 << 7); // do not wrap around receive buffer
    IO::out32(m_device_metadata.io_base_address + RCR_REG, RULES | NO_WRAP);
}

void RTL8139NetworkCard::setup_interrupt_mask()
{
    IO::out16(m_device_metadata.io_base_address + IMR_REG, 0x5); // Accept "Transmit OK" and "Receive OK" interrupts
}

void RTL8139NetworkCard::init_recv_buffer()
{
    uint32_t receive_buffer {0};
    // We want 8K + 16 + 1500 (for no-wrap mode), so 3 pages.
    const uint32_t num_pages = 3;
    if (!MemoryManager::the().get_contigous_free_physical_frames(num_pages, receive_buffer))
    {
        kprintf("Failed to allocate memory for receive buffer\n");
        ASSERT_NOT_REACHED();
    }
    kprintf("receive buffer: %p\n", receive_buffer);
    IO::out32(m_device_metadata.io_base_address + RECEIVEBUFFER_START_REG, receive_buffer);
    m_recv_buffer = reinterpret_cast<uint8_t*>(receive_buffer);
}


void RTL8139NetworkCard::verify_bus_mastering()
{
    uint16_t config_command_reg = PCIBus::config_read_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4);
    kprintf("config_command: %x\n", config_command_reg);
    ASSERT(config_command_reg & 0x2); // Ensure "bus mastering" is on
}

void RTL8139NetworkCard::turn_on()
{
    IO::outb(m_device_metadata.io_base_address + CONFIG_REG_1, 0);
}

void RTL8139NetworkCard::software_reset()
{
    IO::outb(m_device_metadata.io_base_address + COMMAND_REG, 0x10);
    uint32_t reset_bit = 0x4;
    while ((IO::inb(m_device_metadata.io_base_address + COMMAND_REG) & (1<<reset_bit)) != 0) {
        kprintf("waiting for reset\n");
    }
    kprintf("done waiting for reset\n");
}