#pragma once
#include "../../../external/printf/printf.h"
#include "../../../SharedDefs.h"

typedef unsigned long long int uword_t;

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame* frame);