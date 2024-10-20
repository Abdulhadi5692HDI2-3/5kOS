
#include "pci.h"


namespace PCI {
    const char* DeviceClasses[] {
        "Unclassified/Unknown",
        "Mass Storage Controller",
        "Network Adapter",
        "Display Adapter",
        "Multimedia Adapter",
        "Memory Controller",
        "Bridge Device",
        "Simple Communication Controller",
        "Base System Peripheral",
        "Input Device Adapter",
        "Docking Station", 
        "Processor/CPU device",
        "Serial Bus Controller",
        "Wireless Adapter",
        "Intelligent Controller",
        "Satellite Communication Controller",
        "Encryption Controller",
        "Signal Processing Controller",
        "Processing Accelerator",
        "Non Essential Instrumentation"
    };
    const char* GetVendorName(uint16_t VendorId) {
        switch (VendorId) {
            case 0x8086:
                return "Intel Corporation";
            case 0x1022:
                return "AMD";
            case 0x10DE:
                return "NVIDIA Corporation";
            case 0x168C:
                return "Qualcomm Atheros";
            case 0x10EC:
                return "Realtek Semiconductor Co., Ltd";
            default:
                return "Unspecified";
        }
        return "Unspecified";
    }
    const char* GetDeviceName(uint16_t VendorId, uint16_t DeviceId) {
        switch (VendorId) {
            case 0x8086:
                switch (DeviceId) {
                    case 0x29C0:
                        return "Express DRAM Controller";
                    case 0x2918:
                        return "LPC Interface Controller";
                    case 0x2922:
                        return "6 port SATA controller [AHCI mode]";
                    case 0x2930:
                        return "SMBus controller";
                    case 0x31a8:
                        return "Celeron/Pentium Silver Processor USB 3.0 xHCI Controller";
                    case 0x184931a8:
                        return "Celeron/Pentium Silver Processor USB 3.0 xHCI Controller";
                    case 0x31d6:
                        return "Gemini Lake PCI Express Root Port";
                    case 0x31d7:
                        return "Gemini Lake PCI Express Root Port [2]";
                    case 0x3198:
                        return "Celeron/Pentium Silver Processor High Definition Audio";
                    case 0x3195:
                        return "Intel GeminiLake [UHD Graphics 600]";
                }
            case 0x168c:
                switch(DeviceId) {
                    case 0x0042:
                        return "Qualcomm Atheros QCA9377 802.11ac Wireless Network Adapter";
                }
            case 0x10ec:
                switch (DeviceId) {
                    case 0x8136:
                        return "PCI Express Fast Ethernet controller";
                }
        }
        return "Unknown/Unspecified";
    }
}