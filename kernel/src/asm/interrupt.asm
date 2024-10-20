[BITS 64]
section .text
global _lidt
global _sidt
global _sti
global _cli

_sidt:
    sidt [rdi]
    ret

_lidt:
    lidt [rdi]
    ret

_sti:
    sti
    ret

_cli:
    cli
    ret