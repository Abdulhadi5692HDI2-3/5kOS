#include "pci.h"
#include "../memory/paging.h"
#include "../../../external/printf/printf.h"

namespace PCI {
    void EnumerateFunction(uint64_t devAddr, uint64_t function) {
      //  printf("was in enumeratefunction\n");
        uint64_t offset = function << 12;
        uint64_t functionAddr = devAddr + offset;
        IdentityMap4Kb(functionAddr);

        PCIDeviceHeader* PciDeviceHeader = (PCIDeviceHeader*)functionAddr;
        if (PciDeviceHeader->DeviceID == 0) return;
        if (PciDeviceHeader->DeviceID == 0xFFFF) return;
        printf("Vendor: %s / ", GetVendorName(PciDeviceHeader->VendorID));
        printf("Device Name: %s / ", GetDeviceName(PciDeviceHeader->VendorID, PciDeviceHeader->DeviceID)); 
        printf("Class: %s\n", DeviceClasses[PciDeviceHeader->Class]);
    }
    void EnumerateDevice(uint64_t busAddr, uint64_t dev) {
        //printf("was in enumeratedevice\n");
        uint64_t offset = dev << 15;
        uint64_t devAddr = busAddr + offset;
        IdentityMap4Kb(devAddr);
        
        PCIDeviceHeader* PciDeviceHeader = (PCIDeviceHeader*)devAddr;
        if (PciDeviceHeader->DeviceID == 0) {return;}
        if (PciDeviceHeader->DeviceID == 0xFFFF) {return;}
        for (uint64_t function = 0; function < 8; function++) {
            EnumerateFunction(devAddr, function);
        }
    }
    void EnumerateBus(uint64_t base, uint64_t bus) {
        //printf("was in enumeratebus!\n");
        uint64_t offset = bus << 20;
        uint64_t busAddr = base + offset;
        IdentityMap4Kb(busAddr);
        PCIDeviceHeader* PciDeviceHeader = (PCIDeviceHeader*)busAddr;
        if (PciDeviceHeader->DeviceID == 0) {return;}
        if (PciDeviceHeader->DeviceID == 0xFFFF) {return;}
        for (uint32_t device = 0; device < 32; device++) {
            EnumerateDevice(busAddr, device);
        }
    }
    void EnumeratePCI(ACPI::MCFGHeader* mcfg) {
        //printf("was in enumeratepci \n");
        int entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
        for (int t = 0; t < entries; ++t) {
            ACPI::DeviceConfig* DevConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
            for (uint64_t bus = DevConfig->StartBus; bus < DevConfig->EndBus; bus++) {
                EnumerateBus(DevConfig->BaseAddress, bus);
            }
        }
    }
}