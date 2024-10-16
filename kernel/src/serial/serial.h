#pragma once
#include <stdint.h>
#include "../asm/kasm.h"

#define _PORT_COM1 0x3f8

class SerialDevice {
    public:
    void Initalize();
    char ReadChar();
    void WriteChar(char);
};