#pragma
#include "../../SharedDefs.h"
typedef struct {
    UINT x;
    UINT y;
} Point;

class EarlyDisplay {
    public:
    Framebuffer* g_framebuffer;
    PSF1_FONT* g_earlyfont;
    Point g_curpos;
    void Initalize(Framebuffer* buf, PSF1_FONT* font);
    void PutChar(UINT color, char c, UINT xoff, UINT yoff);
    void Print(UINT color, const char* string);
};