#pragma once

#include <stdint.h>
#include "../../../SharedDefs.h"
#include "../memory/paging.h"
#include "../gdt/gdt.h"
#include "../interrupt/idt.h"
// general assembly functions
extern "C" void __hcf();
extern "C" BYTE _inb(unsigned short);
extern "C" void _outb(unsigned short, BYTE);
extern "C" void _crash_reboot();
extern "C" void _lpml4(PageTable*);
extern "C" void _lgdt(GdtDescriptor*);
extern "C" int _argstest(int);
extern "C" void _lidt(_Idtr*);
extern "C" void _sti();
extern "C" void _cli();
extern "C" inline void _iowait() {
    _outb(0, 0x80);
}