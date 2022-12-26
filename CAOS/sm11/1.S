#include <asm/unistd_32.h>
        .text
        .global _start

_start:
        sub     $0x100000, %esp
        mov     %esp, %ecx

.loop_readwrite: 
        mov     $__NR_read, %eax
        mov     $0, %ebx
        mov     $0x100000, %edx
        int     $0x80
        cmp     $0, %eax
        jle     .exit
        

        xor     %ebx, %ebx
.loop_replace:
        mov     (%ecx, %ebx), %dl
        cmp     $0x30, %dl
        jb      .skip_char
        cmp     $0x39, %dl
        ja      .skip_char
        movb    $0x30, (%ecx, %ebx)
.skip_char:
        inc     %ebx
        cmp     %ebx, %eax
        jne     .loop_replace
        

        mov     %eax, %edx
        mov     $__NR_write, %eax
        mov     $1, %ebx
        int     $0x80
        jmp     .loop_readwrite
        
.exit:
        mov     $__NR_exit, %eax
        xor     %ebx, %ebx
        int     $0x80        







