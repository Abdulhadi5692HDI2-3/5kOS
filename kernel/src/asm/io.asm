; IO
[bits 64]
global _inb
global _outb
global _iowait

_outb:
    push rbp
    mov rsp, rbp
    mov edx, edi
    mov eax, esi
    mov WORD [rbp-4], dx
    mov BYTE [rbp-8], al
    movzx eax, BYTE [rbp-8]
    movzx edx, WORD [rbp-4]
    out dx, al
    nop
    pop rbp
    ret

_inb:
    push rbp
    mov rbp, rsp
    mov eax, edi
    mov WORD [rbp-0x14], ax
    movzx eax, WORD [rbp-0x14]
    mov edx, eax
    in al, dx
    mov BYTE [rbp-1], al
    movzx eax, BYTE [rbp-1]
    pop rbp
    ret
