#include "RTL8139NetworkCard.h"
#include "asserts.h"
#include "PCIBus.h"
#include "IO.h"
#include "logging.h"
#include "MM/MemoryManager.h"
#include "PIC.h"
#include "cpu.h"
#include "cstring.h"
#include "Network/NetworkManager.h"

#define VENDOR_ID 0x10EC
#define DEVICE_ID 0x8139
#define CONFIG_REG_1 0x52
#define MAC_REG 0x0
#define COMMAND_REG 0x37
#define RECEIVEBUFFER_START_REG 0x30
#define IMR_REG 0x3c
#define RCR_REG 0x44
#define REG_CAPR 0x38 // Current read address
#define TRANSMIT_START_ADDRESS_0 0x20
#define TRANSMIT_STATUS_0 0x10

// #define RTL8139_DBG

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
#ifdef RTL8139_DBG
    kprintf("isr_rtl8139_handler interrupt!\n~~~~~~~\n");
#endif

    auto interrupt_status = IO::in16(RTL8139NetworkCard::the().io_base_address() + 0x3E);
#ifdef RTL8139_DBG
    kprintf("interrupt status: 0x%x\n", interrupt_status);
#endif

    if (interrupt_status & 0x4) {
#ifdef RTL8139_DBG
        kprintf("Transmit OK\n");
    #endif
    }
    if (interrupt_status & 0x1) {
#ifdef RTL8139_DBG
        kprintf("Recv OK\n");
    #endif
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

#ifdef RTL8139_DBG
    kprintf("status: %d, length: %d\n", status, length);
#endif

    if (!(status & 1))
    {
#ifdef RTL8139_DBG
        kprintf("rtl8139: recv error\n");
#endif
        return;
    }

    auto* packet = current_data + 4;
    auto packet_size = length - 4;
#ifdef RTL8139_DBG
    kprintf("packet:\n");
    print_hexdump(packet, packet_size);
#endif

    Network::NetworkManager::the().on_packet_received(packet, packet_size);

    uint32_t recv_buffer_size = 9708; // 8K + 16 + 1500

    m_recv_buffer_offset = ((m_recv_buffer_offset + length + 4 + 3) & ~3);

    IO::out32(m_device_metadata.io_base_address + REG_CAPR, m_recv_buffer_offset - 0x10);

    m_recv_buffer_offset %= recv_buffer_size;
}


RTL8139NetworkCard::RTL8139NetworkCard(PCIBus::PciDeviceMetadata device_metadata)
     : m_device_metadata(device_metadata)
{
   m_irq_nuber = device_metadata.irq_number;

   verify_bus_mastering();
   turn_on();
   software_reset();
   read_mac_address();
   init_recv_buffer();
   init_send_buffers();
   setup_interrupt_mask();
   configure_receive();
}

void RTL8139NetworkCard::read_mac_address()
{
    for (int i = 0; i < 6; ++i)
    {
        m_mac.data[i] = IO::inb(m_device_metadata.io_base_address + MAC_REG + i);
    }
    kprintf("MAC: ");

    for (int i = 0; i < 6; ++i)
    {
        kprintf("%x", m_mac.data[i]);
        if (i != 5)
        {
            kprintf(":");
        }
    }
    kprintf("\n");
}

void RTL8139NetworkCard::transmit(u8* data, size_t size)
{
    // TODO lock

    ASSERT(m_current_transmit_index < NUM_SEND_BUFFERS);

    auto buffer = m_send_buffers[m_current_transmit_index];

    memcpy(buffer->data(), data, size);

    auto transmit_buffer_physical_addr = MemoryManager::the().get_physical_address((u32)(buffer->data()));

    IO::out32(m_device_metadata.io_base_address + TRANSMIT_START_ADDRESS_0 + m_current_transmit_index*4, transmit_buffer_physical_addr);

    auto current_status = IO::in32(m_device_metadata.io_base_address + TRANSMIT_STATUS_0 + m_current_transmit_index*4);
    current_status = (current_status & 0xfffff000) | size;
    current_status = set_bit(current_status, 13, 0); // begin transmit

#ifdef RTL8139_DBG
    kprintf("beginning to transmit\n");
#endif
    IO::out32(m_device_metadata.io_base_address + TRANSMIT_STATUS_0 + m_current_transmit_index*4, current_status);

    while (get_bit(IO::in32(m_device_metadata.io_base_address + TRANSMIT_STATUS_0 + m_current_transmit_index*4), 15) != 1) {
#ifdef RTL8139_DBG
        kprintf("waiting for transmit\n");
#endif
    }
#ifdef RTL8139_DBG
    kprintf("transmit done\n");
#endif

    current_status = IO::in32(m_device_metadata.io_base_address + TRANSMIT_STATUS_0 + m_current_transmit_index*4);
#ifdef RTL8139_DBG
    kprintf("current send status: 0x%x\n", current_status);
#endif

    m_current_transmit_index = (m_current_transmit_index + 1) % NUM_SEND_BUFFERS;
}

void RTL8139NetworkCard::enable_receive_transmit()
{
    IO::outb(m_device_metadata.io_base_address + COMMAND_REG, 0x0C);

    register_interrupt_handler(IRQ_ISRS_BASE + m_device_metadata.irq_number, isr_rtl8139_entry);

    PIC::enable_irq(m_device_metadata.irq_number);
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
    // (hack, works because in the 2nd time around we don't need to allocate a frame for a new page table)

    auto temp_buffer = BigBuffer::allocate(num_pages * PAGE_SIZE, false);
    m_recv_buffer = BigBuffer::allocate(num_pages * PAGE_SIZE, true);

    auto physical_address_of_recv_buffer = MemoryManager::the().get_physical_address((u32)(m_recv_buffer->data()));

    IO::out32(m_device_metadata.io_base_address + RECEIVEBUFFER_START_REG, physical_address_of_recv_buffer);
}


void RTL8139NetworkCard::verify_bus_mastering()
{
    uint16_t config_command_reg = PCIBus::config_read_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4);

    config_command_reg |= (1 << 0);
    config_command_reg |= (1 << 2); // Enable "bus mastering"

    PCIBus::config_write_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4, config_command_reg);

    config_command_reg = PCIBus::config_read_short(m_device_metadata.pci_bus, m_device_metadata.pci_slot, 0, 0x4);

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

#ifdef RTL8139_DBG
        kprintf("waiting for reset\n");
#endif
    }
#ifdef RTL8139_DBG
    kprintf("done waiting for reset\n");
#endif
}

void RTL8139NetworkCard::init_send_buffers()
{
    for (size_t i = 0; i < NUM_SEND_BUFFERS; ++i)
    {
        m_send_buffers[i] = BigBuffer::allocate(PAGE_SIZE);
        ASSERT(m_send_buffers[i].get() != nullptr);
    }
    m_current_transmit_index = 0;
}
