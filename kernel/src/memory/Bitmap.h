#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../../../SharedDefs.h"

#define STORED_SIZE unsigned long

class Bitmap {
    public:
    STORED_SIZE Size;
    uint8_t* Buffer;
    bool operator[](uint64_t index);
    void Set(uint64_t index, bool value);
};