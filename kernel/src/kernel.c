#include "../../SharedDefs.h"

typedef struct {
    UINT x;
    UINT y;
} Point;

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

Framebuffer* g_framebuffer;
PSF1_FONT* g_earlyfont;
// draw a line
// borrowed from the bootloader
void TestFramebufferPixels(Framebuffer* buf) {
	UINT y = 50;
	UINT BBP = 4;
	for (UINT x = 0; x < buf->Width / 2 * BBP; x++) {
		*(UINT*)(x + (y * buf->PixelsPerScanLine * BBP) + buf->Address) = 0xffffffff;
	}
}

void KeFrameBufferPutChar(UINT color, char c, UINT xoff, UINT yoff) {
    UINT* pixPtr = (UINT*)g_framebuffer->Address;
    char* fontPtr = g_earlyfont->GlyphBuffer + (c * g_earlyfont->Header->charsize);
    for (unsigned long y = yoff; y < yoff + 16; y++) {
        for (unsigned long x = xoff; x < xoff + 8; x++) {
            if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0) {
                *(UINT*)(pixPtr + x + (y * g_framebuffer->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}
void KeStartup(BootParams LoaderParams) {
    g_framebuffer = LoaderParams.bootframebuffer;
    g_earlyfont = LoaderParams.bootfont;
    KeFrameBufferPutChar(RGB(255, 0, 0), 'R', 500, 500);
    KeFrameBufferPutChar(RGB(0, 255, 0), 'G', 510, 500);
    KeFrameBufferPutChar(RGB(0, 0, 255), 'B', 520, 500);
}
void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
}