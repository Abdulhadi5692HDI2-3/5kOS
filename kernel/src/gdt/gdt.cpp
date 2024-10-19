#include "gdt.h"
#include "../asm/kasm.h"

__attribute__((aligned(4096)))
GDT DefaultGDT = {
    {0, 0, 0, 0x00, 0x00, 0}, // null segment
    {0, 0, 0, 0x9a, 0xa0, 0}, // kernel code 64 segment
    {0, 0, 0, 0x92, 0xa0, 0}, // kernel data 64 segment
    {0, 0, 0, 0x00, 0x00, 0}, // user null segment
    {0, 0, 0, 0x9a, 0xa0, 0}, // invalid values for user!
    {0, 0, 0, 0x92, 0xa0, 0}, // invalid values for user!
};

void SetupGDTSegments() {
    GdtDescriptor Descriptor;
    Descriptor.Size = sizeof(GDT) - 1;
    Descriptor.Offset = (uint64_t)&DefaultGDT;
    _lgdt(&Descriptor);
}