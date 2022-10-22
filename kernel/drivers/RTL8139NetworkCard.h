#pragma once

#include "types.h"
#include "constants.h"
#include "PCIBus.h"
#include "BigBuffer.h"
#include "NetworkTypes.h"

class RTL8139NetworkCard
{
public:
    static void initialize();
    static RTL8139NetworkCard& the();


    uint16_t irq_number() const { return m_irq_nuber;}
    uint32_t io_base_address() const { return m_device_metadata.io_base_address;}

    ~RTL8139NetworkCard() = default;

    static void recv_packet_static();

    void transmit(u8* data, size_t size);

    Network::MAC mac() const {return m_mac;}
    void enable_receive_transmit();

private:
    RTL8139NetworkCard(PCIBus::PciDeviceMetadata);

    void verify_bus_mastering();
    void turn_on();
    void software_reset();
    void read_mac_address();
    void init_recv_buffer();
    void init_send_buffers();
    void setup_interrupt_mask();
    void configure_receive();
    void recv_packet();

    static constexpr size_t NUM_SEND_BUFFERS = 4;

    const PCIBus::PciDeviceMetadata m_device_metadata {};
    shared_ptr<BigBuffer> m_recv_buffer {0};
    uint16_t m_recv_buffer_offset {0};
    uint16_t m_irq_nuber {0};
    Network::MAC m_mac = {};
    shared_ptr<BigBuffer> m_send_buffers[NUM_SEND_BUFFERS];
    uint32_t m_current_transmit_index {0};
};
