#include "../../SharedDefs.h"

typedef struct {
    UINT x;
    UINT y;
} Point;

#define RGB(r, g, b) ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff)

Framebuffer* g_framebuffer;
PSF1_FONT* g_earlyfont;
Point g_curpos;
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

void KePrint(UINT color, char* string) {
    char* chr = string;
    while (*chr != 0) {
        if (*chr == '\n') {
            g_curpos.y += 16;
            g_curpos.x = 0;
        }
        KeFrameBufferPutChar(color, *chr, g_curpos.x, g_curpos.y);
        g_curpos.x+=8;
        if (g_curpos.x + 8 > g_framebuffer->Width) {
            g_curpos.x = 0;
            g_curpos.y += 16;
        }
        chr++;
    }
}
void KeStartup(BootParams LoaderParams) {
    g_framebuffer = LoaderParams.bootframebuffer;
    g_earlyfont = LoaderParams.bootfont;
}
void _start(BootParams BootParameters) {
    KeStartup(BootParameters);
    KePrint(RGB(255, 255, 255), "HALLO WORLD!\nHI!");
}