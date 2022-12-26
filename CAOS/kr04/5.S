#include <asm/unistd_32.h>
#define BUF_SIZE 4096
        .text
        .global _start

.readfile:
        mov     $__NR_brk, %eax
        xor     %ebx, %ebx
        int     $0x80
        mov     %eax, %esi
        lea     1(%eax), %ebx
        mov     $__NR_brk, %eax
        int     $0x80
        movb    $0x0A, -1(%eax)
        mov     %eax, %edi

.read_loop:
        mov     $__NR_brk, %eax
        mov     %edi, %ebx
        add     $BUF_SIZE, %ebx
        int     $0x80
        
        mov     $__NR_read, %eax
        xor     %ebx, %ebx
        mov     %edi, %ecx
        mov     $BUF_SIZE, %edx
        int     $0x80

        add     %eax, %edi
        cmp     $0, %eax
        jg      .read_loop
        jl      .error
       
        cmp     %esi, %edi
        je      .readfile_return
        cmpb    $0x0A, -1(%edi)
        je      .readfile_return
        cmp     $BUF_SIZE, %eax
        jne     .add_newline

        mov     $__NR_brk, %eax
        lea     1(%edi), %ebx
        int     $0x80
.add_newline:
        movb    $0x0A, (%edi)
        inc     %edi
.readfile_return:
        inc     %esi
        ret


.writeall:
        ret
        push    %ecx
        push    %edx
        mov     $__NR_write, %eax
        mov     $1, %ebx
        int     $0x80
        cmp     $0, %eax
        jl      .error
        
        pop     %edx
        pop     %ecx
        add     %eax, %ecx
        sub     %eax, %edx
        test    $-1, %edx
        jnz     .writeall
        
        ret




_start:
        call    .readfile
        xor     %edx, %edx

.print_loop:
        cmp     %esi, %edi
        je      .print_last_string
        inc     %edx
        dec     %edi
        cmpb    $0x0A, -1(%edi)
        jne     .print_loop
        
        mov     %edi, %ecx
        call    .writeall
        xor     %edx, %edx
        jmp     .print_loop


.print_last_string:
        mov     %esi, %ecx
        call    .writeall
        call    .exit


.exit:
        xor     %eax, %eax   
.error:
        mov     %eax, %ebx
        mov     $__NR_exit, %eax
        int     $0x80


