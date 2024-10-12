#include "../../SharedDefs.h"
#include "EarlyDisplay.h"
#include "LibKrnl/libkrnl.h"

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

using namespace NSP_EarlyDisplay;
//using namespace krnlstd;

void KeStartup(BootParams LoaderParams) {
    NSP_EarlyDisplay::EarlyDisplay DisplayInterface;
    DisplayInterface.Initalize(LoaderParams.bootframebuffer, LoaderParams.bootfont);
    DisplayInterface.Print(RGB(255, 255, 255), "ohio\n");
    DisplayInterface.Print(RGB(255, 255, 255), ToString(1234));
    DisplayInterface.Print(RGB(255, 255, 255), "\n");
    DisplayInterface.Print(RGB(255,255,255), ToHexString(0xB000));

}
extern "C" void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
}