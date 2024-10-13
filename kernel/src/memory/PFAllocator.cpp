#include "PFAllocator.h"
#include "GeneralMemory.h"

uint64_t FreeMemory;
uint64_t ReservedMemory;
uint64_t UsedMemory;
uint64_t TotalMemory;
bool Initalized = false;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* Map, unsigned long MapSize, unsigned long MapDescSize) {
    if (Initalized) return;
    Initalized = true;
    uint64_t MapEntries = MapSize / MapDescSize;

    void* LargestFreeMemorySegment = NULL;
    unsigned long LargestFreeMemorySize = 0;

    for (UINT i = 0; i < MapEntries; i++) {
        EFI_MEMORY_DESCRIPTOR* Desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)Map + (i * MapDescSize));
        if (Desc->Type == EfiConventionalMemory) {
            if (Desc->NumberOfPages * 4096 > LargestFreeMemorySize) {
                LargestFreeMemorySegment = (void*)Desc->PhysicalStart;
                LargestFreeMemorySize = Desc->NumberOfPages * 4096;
            }
        }
    }
    uint64_t MemorySize = GetMemorySize(Map, MapEntries, MapDescSize);
    FreeMemory = MemorySize;
    TotalMemory = MemorySize;
    uint64_t BitmapSize = MemorySize / 4096 / 8 + 1;
    InitBitmap(BitmapSize, LargestFreeMemorySegment);
    LockPages(&PageBitmap, PageBitmap.Size / 4096 + 1);

    for (UINT i = 0; i < MapEntries; ++i) {
        EFI_MEMORY_DESCRIPTOR* Desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)Map + (i * MapDescSize));
        if (Desc->Type != EfiConventionalMemory) {
            ReservePages((void*)Desc->PhysicalStart, Desc->NumberOfPages);
        }
    }
}

void PageFrameAllocator::InitBitmap(unsigned long BitmapSize, void* Buffer) {
    PageBitmap.Size = BitmapSize;
    PageBitmap.Buffer = (uint8_t*)Buffer;
    for (UINT i = 0; i < BitmapSize; i++) {
        *(uint8_t*)(PageBitmap.Buffer + i) = 0;
    }
}
void* PageFrameAllocator::RequestPage() {
    for (UINT index = 0; index < PageBitmap.Size * 8; index++) {
        if (PageBitmap[index] == true) continue;
        LockPage((void*)(index * 4096));
        return (void*)(index * 4096);
    }
    return NULL;
}

void PageFrameAllocator::FreePage(void* Address) {
    UINTN Index = (UINTN)Address / 4096;
    if (PageBitmap[Index] == false) return;
    PageBitmap.Set(Index, false);
    FreeMemory += 4096;
    UsedMemory -= 4096;
}

void PageFrameAllocator::FreePages(void* Address, UINT Count) {
    for (UINT c = 0; c < Count; c++) {
        FreePage((void*)((uint64_t)Address + (c * 4096)));
    }
}

void PageFrameAllocator::LockPage(void* Address) {
    UINTN Index = (UINTN)Address / 4096;
    if (PageBitmap[Index] == true) return;
    PageBitmap.Set(Index, true);
    FreeMemory -= 4096;
    UsedMemory += 4096;
}

void PageFrameAllocator::LockPages(void* Address, UINT Count) {
    for (UINT c = 0; c < Count; c++) {
        LockPage((void*)((uint64_t)Address + (c * 4096)));
    }
}

void PageFrameAllocator::UnreservePage(void* Address) {
    UINTN Index = (UINTN)Address / 4096;
    if (PageBitmap[Index] == false) return;
    PageBitmap.Set(Index, false);
    FreeMemory += 4096;
    ReservedMemory -= 4096;
}

void PageFrameAllocator::UnreservePages(void* Address, UINT Count) {
    for (UINT c = 0; c < Count; c++) {
        UnreservePage((void*)((uint64_t)Address + (c * 4096)));
    }
}

void PageFrameAllocator::ReservePage(void* Address) {
    UINTN Index = (UINTN)Address / 4096;
    if (PageBitmap[Index] == true) return;
    PageBitmap.Set(Index, true);
    FreeMemory -= 4096;
    ReservedMemory += 4096;
}

void PageFrameAllocator::ReservePages(void* Address, UINT Count) {
    for (UINT c = 0; c < Count; c++) {
        ReservePage((void*)((uint64_t)Address + (c * 4096)));
    }
}


uint64_t PageFrameAllocator::GetFreeRAM(){
    return FreeMemory;
}
uint64_t PageFrameAllocator::GetUsedRAM(){
    return UsedMemory;
}
uint64_t PageFrameAllocator::GetReservedRAM(){
    return ReservedMemory;
}

uint64_t PageFrameAllocator::GetTotalRAM() {
    return TotalMemory;
}