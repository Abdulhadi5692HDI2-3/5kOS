#include "GeneralMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* Map, uint64_t MapEntries, uint64_t MapDescSize){

    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < MapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)Map + (i * MapDescSize));
        memorySizeBytes += desc->NumberOfPages * 4096;
    }

    return memorySizeBytes;

}


void * MemSetZero(void * s, uint64_t n) {
    unsigned char* ptr = (unsigned char* )s;
    for (int i = 0; i < n; i++) {
        *ptr++ = 0;
    }
}