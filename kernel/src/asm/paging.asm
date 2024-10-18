[BITS 64]
section .text

global _lpml4

_lpml4:
    mov rdi, cr3
    ret