#include "interrupt.h"
#include "../debug.h"
#include "../asm/kasm.h"
#include "../serial/serial.h"

// 0xE
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame) {
    KeBugCheck("Page Fault occured! System halting...\n");
    __hcf();
}
// 0xD
__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame* frame) {
    KeBugCheck("General Protection Fault! System halting...\n");
    __hcf();
}
// 0x8
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame) {
    KeBugCheck("Double Fault! System halting..\n");
    __hcf();
}
// 0x21
__attribute__((interrupt)) void KBD_Handler(struct interrupt_frame* frame) {
    uint8_t scancode = _inb(0x60);
    printf("0x%x ", scancode);
    PIC_EndMaster();
}
// 0x3
__attribute__((interrupt)) void Breakpoint_Handler(struct interrupt_frame* frame) {
    printf("breakpoint reached! (int3 instruction)\n waiting for input from COM1\n");
    uint8_t wait = DefaultSerialDevice.ReadChar();
    printf("resuming code execution...\n");
}

// undefined
__attribute__((interrupt)) void Undefined_Handler(struct interrupt_frame* frame) {
    printf("Unhandled interrupt with no handler!\n");
    printf("resuming code execution...\n");
}

void PIC_EndMaster() {
    _outb(PIC1_COMMAND, PIC_EOI);
}
void PIC_EndSlave() {
    _outb(PIC2_COMMAND, PIC_EOI);
    PIC_EndMaster();
}

void PIC_SetMask(uint8_t Irq) {
    unsigned short port;
    uint8_t val;
    if (Irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        Irq -= 8;
    }
    val = _inb(port) | (1 << Irq);
    _outb(port, val);
}

void PIC_ClearMask(uint8_t Irq) {
    unsigned short port;
    uint8_t val;
    if (Irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        Irq -= 8;
    }
    val = _inb(port) | ~(1 << Irq);
    _outb(port, val);       
}
void RemapPIC() {
    uint8_t a1, a2;

    a1 = _inb(PIC1_DATA);
    _iowait();
    a2 = _inb(PIC2_DATA);

    _outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    _iowait();
    _outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    _outb(PIC1_DATA, 0x20);
    _iowait();
    _outb(PIC2_DATA, 0x28);

    _outb(PIC1_DATA, 4);
    _iowait();
    _outb(PIC2_DATA, 2);

    _outb(PIC1_DATA, ICW4_8086);
    _iowait();
    _outb(PIC2_DATA, ICW4_8086);

    _outb(PIC1_DATA, a1);
    _iowait();
    _outb(PIC2_DATA, a2);
}