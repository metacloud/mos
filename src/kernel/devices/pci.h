#ifndef DEVICE_PCI_H
#define DEVICE_PCI_H

#include <stdint.h>

#define PCI_INVALID_VENDOR_ID 0xFFFF
#define PCI_MULTIFUNCTION_DEVICE 0x80

#define PCI_CLASS_CODE_MASS_STORAGE 0x01
#define PCI_CLASS_CODE_BRIDGE_DEVICE 0x06

#define PCI_SUBCLASS_IDE 0x01
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE 0x04

void pci_scan_bus(uint8_t bus);
void pci_scan_buses();

#endif