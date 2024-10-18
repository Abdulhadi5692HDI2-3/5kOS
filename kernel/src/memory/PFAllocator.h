#pragma once

#include <stdint.h>
#include "../../../SharedDefs.h"
#include "Bitmap.h"
class PageFrameAllocator {
    public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* Map, unsigned long MapSize, unsigned long MapDescSize);
    Bitmap PageBitmap;
    void InitBitmap(unsigned long BitmapSize, void* Buffer) ;
    void* RequestPage();
    void FreePage(void* Address);
    void FreePages(void* Address, UINT Count);
    void LockPage(void* Address);
    void LockPages(void* Address, UINT Count);
    void UnreservePage(void* Address);
    void UnreservePages(void* Address, UINT Count);
    void ReservePage(void* Address);
    void ReservePages(void* Address, UINT Count);
    uint64_t GetFreeRAM();
    uint64_t GetUsedRAM();
    uint64_t GetReservedRAM();
    uint64_t GetTotalRAM();
};

extern PageFrameAllocator GlobalAllocator;