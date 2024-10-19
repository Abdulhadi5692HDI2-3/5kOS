#pragma once
#include <stdint.h>


// Segmentation is useless in 64 bit long mode
// but for some reason it is still required??
typedef struct {
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed)) GdtDescriptor;

typedef struct {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
} __attribute__((packed)) GdtEntry;

typedef struct {
    GdtEntry Null;
    GdtEntry KernelCode;
    GdtEntry KernelData;
    GdtEntry UserNull;
    GdtEntry UserCode;
    GdtEntry UserData;
} __attribute__((packed)) GDT;
__attribute__((aligned(4096)))
extern GDT DefaultGDT;

void SetupGDTSegments();