#include "debug.h"
#include <stdarg.h>
#include "../../external/printf/printf.h"
#include "EarlyDisplay.h"
#include "serial/serial.h"
#include "LibKrnl/libkrnl.h"

void KeBugCheck(const char* string, ...) {
    va_list va;
    va_start(va, string);
    DisplayInterface.Print(((255 & 0xff) << 16) + ((0 & 0xff) << 8) + (0 & 0xff), "STOP: Kernel bugcheck!\n");
    vprintf_(string, va);
    printf("\n  !!! Send any input to the COM1 terminal to reset the system !!! (has a delay)\n");
    char in = DefaultSerialDevice.ReadChar();
    _crash_reboot();
    va_end(va);
}

void KeAssertEx(bool condition, const char* file, int line) {
    if (!condition) {
        //KeDbgPrint("KeAssertEx: Assert failed at %s:%d!\n", file, line);
        printf("Assert failed at %s:%d!\n", file, line);
    }
}