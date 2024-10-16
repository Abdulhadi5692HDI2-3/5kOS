#pragma once

#include <stdint.h>
#include "../../../SharedDefs.h"

// general assembly functions
extern "C" void __hcf();
extern "C" BYTE _inb(unsigned short);
extern "C" void _outb(unsigned short, BYTE);