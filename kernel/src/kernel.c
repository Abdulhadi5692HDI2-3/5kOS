#include "../../SharedDefs.h"
// draw a line
// borrowed from the bootloader
void TestFramebufferPixels(Framebuffer* buf) {
	UINT y = 50;
	UINT BBP = 4;
	for (UINT x = 0; x < buf->Width / 2 * BBP; x++) {
		*(UINT*)(x + (y * buf->PixelsPerScanLine * BBP) + buf->Address) = 0xffffffff;
	}
}
void KeStartup(BootParams LoaderParams) {
    
}
void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
}