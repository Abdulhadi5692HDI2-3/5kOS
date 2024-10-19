#include "interrupt.h"
#include "../debug.h"
#include "../asm/kasm.h"

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