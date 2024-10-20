#pragma once


#include <stdint.h>

#ifdef __cplusplus
namespace ACPI {
    typedef struct {
        unsigned char Signature[8];
        uint8_t Checksum;
        unsigned char OEMID[6];
        uint8_t Revision;
        uint32_t RsdtAddress;
        uint32_t Length;
        uint64_t XsdtAddress;
        uint8_t ExtendedChecksum;
        uint8_t reserved[3];
    } __attribute__((packed)) RDSP2;
    typedef struct {
        unsigned char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        unsigned char OEMID[6];
        unsigned char OEMTableID[8];
        uint32_t OEMRevision;
        uint32_t CreatorID;
        uint32_t CreatorRevision;
    } __attribute__((packed)) XSDTHeader;
    typedef struct {
        XSDTHeader Header;
        uint64_t Unused;
    } __attribute__((packed)) MCFGHeader;

    typedef struct {
        uint64_t BaseAddress;
        uint16_t PCISegGroup;
        uint8_t StartBus;
        uint8_t EndBus;
        uint32_t Reserved;
    }__attribute__((packed)) DeviceConfig;
    void* FindTable(XSDTHeader*, char*);
}
#else
typedef struct {
        unsigned char Signature[8];
        uint8_t Checksum;
        unsigned char OEMID[6];
        uint8_t Revision;
        uint32_t RsdtAddress;
        uint32_t Length;
        uint64_t XsdtAddress;
        uint8_t ExtendedChecksum;
        uint8_t reserved[3];
    } __attribute__((packed)) RDSP2;
    typedef struct {
        unsigned char Signature[4];
        uint32_t Length;
        uint8_t Revision;
        uint8_t Checksum;
        unsigned char OEMID[6];
        unsigned char OEMTableID[8];
        uint32_t OEMRevision;
        uint32_t CreatorID;
        uint32_t CreatorRevision;
    } __attribute__((packed)) XSDTHeader;
    typedef struct {
        XSDTHeader Header;
        uint64_t Unused;
    } __attribute__((packed)) MCFGHeader;
#endif