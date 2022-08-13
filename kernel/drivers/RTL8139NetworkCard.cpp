#include "RTL8139NetworkCard.h"
#include "asserts.h"
#include "PCIBus.h"
#include "IO.h"
#include "logging.h"
#include "MM/MemoryManager.h"
#include "PIC.h"
#include "cpu.h"
#include "cstring.h"

#define VENDOR_ID 0x10EC
#define DEVICE_ID 0x8139
#define CONFIG_REG_1 0x52
#define COMMAND_REG 0x37
#define RECEIVEBUFFER_START_REG 0x30
#define IMR_REG 0x3c
#define RCR_REG 0x44
#define REG_CAPR 0x38 // Current read address

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
    kprintf("isr_rtl8139_handler interrupt!\n~~~~~~~\n");

    auto interrupt_status = IO::in16(RTL8139NetworkCard::the().io_base_address() + 0x3E);
    kprintf("interrupt status: 0x%x\n", interrupt_status);

    if (interrupt_status & 0x4) {
        kprintf("Transmit OK\n");
    }
    if (interrupt_status & 0x1) {
        kprintf("Recv OK\n");
        RTL8139NetworkCard::recv_packet_static();
    }

    IO::out16(RTL8139NetworkCard::the().io_base_address() + 0x3E, 0x5);
    PIC::eoi(RTL8139NetworkCard::the().irq_number());
}

void RTL8139NetworkCard::recv_packet_static()
{

    RTL8139NetworkCard::the().recv_packet();
}

void RTL8139NetworkCard::recv_packet()
{
    // This follows the psuedo-code in realtek's RTL8139 programming guide

    auto* current_data = m_recv_buffer->data() + m_recv_buffer_offset;
    uint16_t status = *(uint16_t*)(current_data);
    uint16_t length = *(uint16_t*)(current_data + 2);
    kprintf("status: %d, length: %d\n", status, length);

    if (!(status & 1))
    {
        kprintf("rtl8139: recv error\n");
        return;
    }

    auto* packet = current_data + 4;
    auto packet_size = length - 4;
    kprintf("packet:\n");
    print_hexdump(packet, packet_size);

    uint32_t recv_buffer_size = 9708; // 8K + 16 + 1500

    m_recv_buffer_offset = ((m_recv_buffer_offset + length + 4 + 3) & ~3);

    IO::out32(m_device_metadata.io_base_address + REG_CAPR, m_recv_buffer_offset - 0x10);

    m_recv_buffer_offset %= recv_buffer_size;
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
   setup_interrupt_mask();
   configure_receive();
   enable_receive_transmit();
   register_interrupt_handler(IRQ_ISRS_BASE + device_metadata.irq_number, isr_rtl8139_entry);

    PIC::enable_irq(device_metadata.irq_number);


    auto transmit_buffer = BigBuffer::allocate(PAGE_SIZE);
    char message[] = {1,2,3,4,5};
    memcpy(transmit_buffer->data(), message, 5);

    auto transmit_buffer_physical_addr = MemoryManager::the().get_physical_address((u32)(transmit_buffer->data()));
    kprintf("message buffer physical addr: %p\n", transmit_buffer_physical_addr);
    kprintf("message buffer content: 0x%x\n", *((uint32_t*)transmit_buffer->data()));

    IO::out32(m_device_metadata.io_base_address + 0x20, transmit_buffer_physical_addr);

    auto current_status = IO::in32(m_device_metadata.io_base_address + 0x10);
    kprintf("current send status: 0x%x\n", current_status);
    current_status = (current_status & 0xfffff000) | 0x5; // size = 5
    current_status = set_bit(current_status, 13, 0); // begin transmit

    kprintf("beginning to transmit\n");
    IO::out32(m_device_metadata.io_base_address + 0x10, current_status);

    while (get_bit(IO::in32(m_device_metadata.io_base_address + 0x10), 15) != 1) {
        kprintf("waiting for transmit\n");
    }
    kprintf("transmit done\n");

    current_status = IO::in32(m_device_metadata.io_base_address + 0x10);
    kprintf("current send status: 0x%x\n", current_status);
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
    // We want 8K + 16 + 1500 (for no-wrap mode), so 3 pages.
    const uint32_t num_pages = 3;


    // NOTE: Terrible hack.
    // Allocate twice to actually get contiguous physical memory
    // (by luck, since in the 2nd time around we don't need to allocate a frame for the page table)

    auto temp_buffer = BigBuffer::allocate(num_pages * PAGE_SIZE, false);
    m_recv_buffer = BigBuffer::allocate(num_pages * PAGE_SIZE, true);

    auto physical_address_of_recv_buffer = MemoryManager::the().get_physical_address((u32)(m_recv_buffer->data()));

    IO::out32(m_device_metadata.io_base_address + RECEIVEBUFFER_START_REG, physical_address_of_recv_buffer);
    // kprintf("receive buffer: %p\n", m_recv_buffer->data());
    // m_recv_buffer->data()[0] = 1;
    // kprintf("receive buffer[0]: %d\n", m_recv_buffer->data()[0]);
}


void RTL8139NetworkCard::verify_bus_mastering()
{
    uint16_t config_command_reg = PCIBus::config_read_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4);
    kprintf("config_command: 0x%x\n", config_command_reg);

    config_command_reg |= (1 << 0);
    config_command_reg |= (1 << 2); // Enable "bus mastering"

    PCIBus::config_write_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4, config_command_reg);

    config_command_reg = PCIBus::config_read_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4);
    kprintf("config_command after enabling bus mastering: 0x%x\n", config_command_reg);

    ASSERT(config_command_reg & (1<<2)); // Ensure "bus mastering" is on
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