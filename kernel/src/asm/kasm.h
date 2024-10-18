#pragma once

#include <stdint.h>
#include "../../../SharedDefs.h"
#include "../memory/paging.h"

// general assembly functions
extern "C" void __hcf();
extern "C" BYTE _inb(unsigned short);
extern "C" void _outb(unsigned short, BYTE);
extern "C" void _crash_reboot();
extern "C" void _lpml4(PageTable* PML4);
extern "C" int _argstest(int);