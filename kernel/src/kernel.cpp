#include "../../SharedDefs.h"
#include "EarlyDisplay.h"
#include "LibKrnl/libkrnl.h"
#include "asm/kasm.h"

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

using namespace NSP_EarlyDisplay;
//using namespace krnlstd;

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
    KePrintMemoryMap(LoaderParams.MemMap, DisplayInterface);
}
extern "C" void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
    // if we ever get here then just halt
    __hcf();
}