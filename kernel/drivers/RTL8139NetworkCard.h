#pragma once

#include "types.h"
#include "constants.h"
#include "PCIBus.h"

class RTL8139NetworkCard
{
public:
    static void initialize();
    static RTL8139NetworkCard& the();


    uint16_t irq_number() const { return m_irq_nuber;}
    uint32_t io_base_address() const { return m_device_metadata.io_base_address;}

    ~RTL8139NetworkCard() = default;

private:
    RTL8139NetworkCard(PCIBus::PciDeviceMetadata);

    void verify_bus_mastering();
    void turn_on();
    void software_reset();
    void init_recv_buffer();
    void setup_interrupt_mask();
    void configure_receive();
    void enable_receive_transmit();

    const PCIBus::PciDeviceMetadata m_device_metadata {};
    uint8_t* m_recv_buffer {0};
    uint16_t m_irq_nuber {0};

};