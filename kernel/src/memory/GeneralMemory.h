#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../../SharedDefs.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* Map, uint64_t MapEntries, uint64_t MapDescSize);

void* MemSet(void* dest, int val, __SIZE_TYPE__ n);
void* MemCpy(void* dest, const void* src, __SIZE_TYPE__ n);