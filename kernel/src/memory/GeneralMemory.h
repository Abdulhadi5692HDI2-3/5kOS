#pragma once
#include <stdint.h>
#include "../../../SharedDefs.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* Map, uint64_t MapEntries, uint64_t MapDescSize);