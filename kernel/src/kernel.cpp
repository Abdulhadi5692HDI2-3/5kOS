#include "../../SharedDefs.h"
#include "EarlyDisplay.h"
#include "LibKrnl/libkrnl.h"
#include "asm/kasm.h"
#include "serial/serial.h"
#include "../../external/printf/printf.h"
#include "debug.h"
#include "memory/PFAllocator.h"
#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

using namespace NSP_EarlyDisplay;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

//using namespace krnlstd;

void _putchar(char character) {
    char array[2] = {character, '\0'};
    DisplayInterface.Print(RGB(255, 255, 255), array);
    DefaultSerialDevice.WriteChar(character);
}

void KePrintMemoryMap(MemoryMap populatemm, NSP_EarlyDisplay::EarlyDisplay Out) {
    ULONGLONG mMapEntries = populatemm.MapSize / populatemm.MapDescriptorSize;
    for (int i = 0; i < mMapEntries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((ULONGLONG)populatemm.Map + (i * populatemm.MapDescriptorSize));
        Out.g_curpos.x = 0;
        Out.g_curpos.y = Out.g_curpos.y + 16;

        Out.Print(RGB(107, 28, 217), EFI_MEMORY_TYPE_STRINGS[desc->Type]);
        Out.Print(RGB(0, 0, 0), " ");
        Out.Print(RGB(255, 255, 255), ToString(desc->NumberOfPages * 4096 / 1024));
        Out.Print(RGB(255, 255, 255), " KB");
    }
}

void KeKernelInitalize(BootParams LoaderParams) {
    SerialDevice COM1;
    COM1.Initalize();
    DefaultSerialDevice = COM1;

    DisplayInterface.Initalize(LoaderParams.bootframebuffer, LoaderParams.bootfont);
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Initalized the framebuffer and display. Framebuffer address: 0x%lx\n", LoaderParams.bootframebuffer->Address);
    #endif
    PageFrameAllocator PFA;
    uint64_t KernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t KernelPages = (uint64_t)KernelSize / 4096 + 1;

    PFA.ReadEFIMemoryMap(LoaderParams.MemMap.Map, LoaderParams.MemMap.MapSize, LoaderParams.MemMap.MapDescriptorSize);
    PFA.LockPages(&_KernelStart, KernelPages);
    #ifdef _INIT_DEBUG
    printf("KeKernelInitalize: Locked %d pages for the kernel!\n", KernelPages);
    #endif
}
void KeStartup(BootParams LoaderParams) {
    KeKernelInitalize(LoaderParams);
    printf("Hello world!\n");
    printf("Boot params magic: 0x%X\n", LoaderParams.Magic);
}
extern "C" void _start(BootParams BootParameters) {
    if (BootParameters.Magic != BOOTPARAM_MAGIC) {
        __hcf();
    }
    KeStartup(BootParameters);
    // if we ever get here then just halt
    __hcf();
}