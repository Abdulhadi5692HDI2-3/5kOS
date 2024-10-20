#pragma once
#include <stdint.h>
#include "../acpi/acpi.h"

namespace PCI {
    typedef struct {
        uint16_t VendorID;
        uint16_t DeviceID;
        uint16_t Command;
        uint16_t Status;
        uint8_t RevisionID;
        uint8_t ProgIF;
        uint8_t Subclass;
        uint8_t Class;
        uint8_t CacheLineSize;
        uint8_t LatencyTimer;
        uint8_t HeaderType;
        uint8_t BIST;
    } PCIDeviceHeader;
    void EnumeratePCI(ACPI::MCFGHeader*);
    extern const char* DeviceClasses[];
    const char* GetVendorName(uint16_t VendorId);
    const char* GetDeviceName(uint16_t VendorId, uint16_t DeviceId);
}