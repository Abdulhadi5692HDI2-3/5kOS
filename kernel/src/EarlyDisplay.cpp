#include "EarlyDisplay.h"

using namespace NSP_EarlyDisplay;
void EarlyDisplay::Initalize(Framebuffer* buf, PSF1_FONT* font) {
    g_earlyfont = font;
    g_framebuffer = buf;
    g_curpos = {0, 0};
}

void EarlyDisplay::PutChar(UINT color, char c, UINT xoff, UINT yoff) {
    UINT* pixPtr = (UINT*)g_framebuffer->Address;
    char* fontPtr = (char*)g_earlyfont->GlyphBuffer + (c * g_earlyfont->Header->charsize);
    for (unsigned long y = yoff; y < yoff + 16; y++) {
        for (unsigned long x = xoff; x < xoff + 8; x++) {
            if ((*fontPtr & (0b10000000 >> (x - xoff))) > 0) {
                *(UINT*)(pixPtr + x + (y * g_framebuffer->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void EarlyDisplay::Print(UINT color, const char* string) {
    char* chr = (char*)string;
    while (*chr != 0) {
        if (*chr == '\n') {
            g_curpos.y += 16;
            g_curpos.x = 0;
        }
        PutChar(color, *chr, g_curpos.x, g_curpos.y);
        g_curpos.x+=8;
        if (g_curpos.x + 8 > g_framebuffer->Width) {
            g_curpos.x = 0;
            g_curpos.y += 16;
        }
        chr++;
    }
}