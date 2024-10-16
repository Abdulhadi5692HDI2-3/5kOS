[BITS 64]

section .data
_idt equ 0
section .text
global _crash_reboot

_crash_reboot:
    lidt [_idt]
    int 0x1