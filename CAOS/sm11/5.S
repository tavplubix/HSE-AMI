#include <asm/unistd_32.h>
        .text
        .global writeu32

writeu32:
        enter   $20, $0
        mov     %edi, 12(%esp)
        mov     %ebx, 16(%esp)

        lea     12(%esp), %edi

.loop_convert:
        mov     $0xCCCCCCCD, %eax
        mul     %ecx
        shr     $3, %edx
        mov     %ecx, %ebx
        mov     %edx, %ecx
        mov     $10, %eax
        mul     %ecx
        sub     %eax, %ebx
        add     $0x30, %ebx
        dec     %edi
        mov     %bl, (%edi)
        test    $-1, %ecx
        jnz     .loop_convert


        lea     12(%esp), %ebx        
.loop_write:
        mov     (%edi), %cl
        mov     stout, %edx
        call    writechar
        inc     %edi
        cmp     %edi, %ebx
        jne     .loop_write


        mov     12(%esp), %edi
        mov     16(%esp), %ebx
        leave
        ret

    

