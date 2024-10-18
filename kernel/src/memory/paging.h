#pragma once

#include <stdint.h>


typedef struct {
    uint64_t entries[512];
} __attribute__((packed)) PageTable;


void IdentityMap4Kb(uint64_t logical);
void SetupPaging();