#include "PCIBus.h"
#include "logging.h"
#include "IO.h"


namespace PCIBus
{

uint16_t config_read_short(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = 0;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    IO::out32(0xCF8, address);

    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((IO::in32(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

struct PciDeviceIdentifier
{
    uint16_t vendor_id {0};
    uint16_t device_id {0};
};

static bool pci_check_device(uint8_t bus, uint8_t slot, PciDeviceIdentifier& out_vendor) {
    uint16_t vendor = config_read_short(bus, slot, 0, 0);
    if (vendor == 0xFFFF)
        return false;


    uint16_t device_id = config_read_short(bus, slot, 0, 2);
    if (vendor == 0xFFFF)
        return false;

    out_vendor.vendor_id = vendor;
    out_vendor.device_id = device_id;

    return true;
}

bool get_device_metadata(uint16_t vendor_id, uint16_t device_id, PciDeviceMetadata& device_metadata)
{
    for (uint32_t bus = 0; bus < 256; ++bus) {
        for (uint32_t device = 0; device < 32; ++device) {
            PciDeviceIdentifier device_vendor_and_id {};
            if (!pci_check_device(bus, device, device_vendor_and_id))
                continue;

            if (device_vendor_and_id.vendor_id != vendor_id || device_vendor_and_id.device_id != device_id)
            {
                continue;
            }

            uint16_t header_type = 0;
            header_type = config_read_short(bus, device, 0, 0xa) & 0xFF;
            if (header_type != 0)
            {
                kprintf("header type: %d\n", header_type);
                continue;
            }
            uint32_t base_address_low = config_read_short(bus, device, 0, 0x10);
            uint32_t base_address_high = config_read_short(bus, device, 0, 0x12);

            uint32_t io_base_address = (base_address_high << 16) | base_address_low;
            kprintf("io base address: %p\n", io_base_address);
            io_base_address &= 0xff00; // I don't know why, but we're reading 1 in the LSB here and it's causing off-by-1 errors.
            kprintf("io base address after mask: %p\n", io_base_address);

            uint16_t irq_number = config_read_short(bus, device, 0, 0x3c) & 0xFF;

            device_metadata.pci_bus = bus;
            device_metadata.pci_slot = device;
            device_metadata.vendor_id = device_vendor_and_id.vendor_id;
            device_metadata.device_id = device_vendor_and_id.device_id;
            device_metadata.io_base_address = io_base_address;
            device_metadata.irq_number = irq_number;
            return true;
        }
    }
    return false;
}

void initialize()
{
    kprintf("PCIBus::Initialize()\n");
    for (uint32_t bus = 0; bus < 256; ++bus) {
        for (uint32_t device = 0; device < 32; ++device) {
            PciDeviceIdentifier device_id {};
            if (!pci_check_device(bus, device, device_id))
                continue;

            kprintf("PCI[%d,%d]: vendor: 0x%x, id: 0x%x\n", bus, device, device_id.vendor_id, device_id.device_id);
        }
    }
}

}