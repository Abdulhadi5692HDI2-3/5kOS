#pragma once
#include "../../../external/printf/printf.h"
#include "../../../SharedDefs.h"

typedef unsigned long long int uword_t;

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void KBD_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Breakpoint_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void Undefined_Handler(struct interrupt_frame* frame);
void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();

void PIC_SetMask(uint8_t Irq);
void PIC_ClearMask(uint8_t Irq);