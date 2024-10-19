[BITS 64]
section .text
global _lidt
global _sti
global _cli
_lidt:
    lidt [rdi]
    ret

_sti:
    sti
    ret

_cli:
    cli
    ret