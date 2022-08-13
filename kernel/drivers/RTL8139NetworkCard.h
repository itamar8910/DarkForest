#pragma once

#include "types.h"
#include "constants.h"
#include "PCIBus.h"
#include "BigBuffer.h"

class RTL8139NetworkCard
{
public:
    static void initialize();
    static RTL8139NetworkCard& the();


    uint16_t irq_number() const { return m_irq_nuber;}
    uint32_t io_base_address() const { return m_device_metadata.io_base_address;}

    ~RTL8139NetworkCard() = default;

    static void recv_packet_static();

private:
    RTL8139NetworkCard(PCIBus::PciDeviceMetadata);

    void verify_bus_mastering();
    void turn_on();
    void software_reset();
    void init_recv_buffer();
    void setup_interrupt_mask();
    void configure_receive();
    void enable_receive_transmit();
    void recv_packet();

    const PCIBus::PciDeviceMetadata m_device_metadata {};
    shared_ptr<BigBuffer> m_recv_buffer {0};
    uint32_t m_recv_buffer_offset {0};
    uint16_t m_irq_nuber {0};

};