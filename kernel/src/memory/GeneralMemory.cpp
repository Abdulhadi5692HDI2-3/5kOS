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


void* MemSet(void* dest, int val, __SIZE_TYPE__ n) {
    unsigned char *ptr = (unsigned char*)dest;

    while (n--) {
        *ptr++ = val;
    }
    return dest;
}

void* MemCpy(void* dest, const void* src, __SIZE_TYPE__ n) {
    const unsigned char* q = (unsigned char*)src;
    unsigned char* ptr = (unsigned char*)dest;
    while (n--) {
        *ptr++ = *q++;
    }
    return dest;
}  