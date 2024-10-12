#include "../../SharedDefs.h"
#include "EarlyDisplay.h"

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)


void KeStartup(BootParams LoaderParams) {
    EarlyDisplay DisplayInterface;
    DisplayInterface.Initalize(LoaderParams.bootframebuffer, LoaderParams.bootfont);
    DisplayInterface.Print(RGB(255, 255, 255), "ohio");
}
extern "C" void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
}