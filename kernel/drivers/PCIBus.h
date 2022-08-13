#pragma once

#include "types.h"
#include "constants.h"

namespace PCIBus
{
    void initialize();

    struct PciDeviceMetadata
    {
        uint16_t pci_bus {0};
        uint16_t pci_slot {0};
        uint16_t vendor_id {0};
        uint16_t device_id {0};
        uint32_t io_base_address {0};
        uint32_t irq_number {0};
    };

    uint16_t config_read_short(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
    bool get_device_metadata(uint16_t vendor_id, uint16_t device_id, PciDeviceMetadata&);
}