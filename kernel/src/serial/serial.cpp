#include "serial.h"
#include <stdarg.h>
#include "../LibKrnl/libkrnl.h"


unsigned int g_port = _PORT_COM1;

SerialDevice DefaultSerialDevice;
void SerialDevice::Initalize() {
    _outb(g_port + 1, 0x00);
    _outb(g_port + 3, 0x80);
    _outb(g_port, 0x03);
    _outb(g_port + 1, 0x00);
    _outb(g_port + 3, 0x03);
    _outb(g_port + 2, 0xC7);
    _outb(g_port + 4, 0x0B);
    _outb(g_port + 4, 0x1E);
    _outb(g_port, 0xAE);

    if (_inb(g_port) != 0xAE) {
        // err
        return;
    }
    _outb(g_port + 4, 0x0F);
    g_port = g_port;
}

int SerialReceived() {
    return _inb(g_port + 5) & 1;
}

int IsTransmitEmpty() {
    return _inb(g_port + 5) & 0x20;
}
char SerialDevice::ReadChar() {
    while (SerialReceived() == 0);
    return _inb(g_port);
}

void SerialDevice::WriteChar(char character) {
    while(IsTransmitEmpty() == 0);
    _outb(g_port, character);
}