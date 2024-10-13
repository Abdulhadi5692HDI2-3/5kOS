#include "../../SharedDefs.h"
#include "EarlyDisplay.h"
#include "LibKrnl/libkrnl.h"
#include "asm/kasm.h"
#include "memory/PFAllocator.h"

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

using namespace NSP_EarlyDisplay;
//using namespace krnlstd;

extern void* _KernelStart;
extern void* _KernelEnd;
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
void KeStartup(BootParams LoaderParams) {
    NSP_EarlyDisplay::EarlyDisplay DisplayInterface;
    DisplayInterface.Initalize(LoaderParams.bootframebuffer, LoaderParams.bootfont);
    //DisplayInterface.Print(RGB(255, 255, 255), "ohio\n"); // todo: \n creates this weird  | character
    // /DisplayInterface.Print(RGB(255, 255, 255), ToString(1234));
    //KePrintMemoryMap(LoaderParams.MemMap, DisplayInterface);
    PageFrameAllocator Allocator;
    Allocator.ReadEFIMemoryMap(LoaderParams.MemMap.Map, LoaderParams.MemMap.MapSize, LoaderParams.MemMap.MapDescriptorSize);
    uint64_t KernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t KernelPages = (uint64_t)KernelSize / 4096 + 1;
    Allocator.LockPages(&_KernelStart, KernelPages);
    DisplayInterface.Print(RGB(255, 255, 255), "Total RAM: ");
    DisplayInterface.Print(RGB(107, 28, 217), ToString(Allocator.GetTotalRAM() / 1024));
    DisplayInterface.Print(RGB(255, 255, 255), " KB, Free RAM: ");
    DisplayInterface.Print(RGB(107, 28, 217), ToString(Allocator.GetFreeRAM() / 1024));
    DisplayInterface.Print(RGB(255, 255,  255), " KB, Used RAM: ");
    DisplayInterface.Print(RGB(107, 28, 217), ToString(Allocator.GetUsedRAM() / 1024));
    DisplayInterface.Print(RGB(255, 255,  255), " KB, Reserved RAM: ");
    DisplayInterface.Print(RGB(107, 28, 217), ToString(Allocator.GetReservedRAM() / 1024));
    DisplayInterface.Print(RGB(255, 255, 255), " KB                         \n"); // new lines create this weird | character tho it still does a newline
    for (UINT t = 0; t < 20; t++) {
        // we're gonna to 20 pages! for science
        void* Addr = Allocator.RequestPage();
        DisplayInterface.Print(RGB(255, 255, 255), "Page no. ");
        DisplayInterface.Print(RGB(255, 255, 255), ToString(t));
        DisplayInterface.Print(RGB(255, 255, 255), "  Address: ");
        DisplayInterface.Print(RGB(255, 255, 255), ToHexString((uint64_t)Addr));
        DisplayInterface.g_curpos = {0, DisplayInterface.g_curpos.y + 16};
    }
}
extern "C" void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
    // if we ever get here then just halt
    __hcf();
}