        .text
        .global addbe

addbe:
        
        enter   $12, $0
        mov     %esi, (%esp)
        mov     %edi, 4(%esp)
        mov     %ebx, 8(%esp)

        mov     8(%ebp), %ecx
        mov     12(%ebp), %esi
        mov     16(%ebp), %edi
        mov     20(%ebp), %ebx

        add     %ecx, %esi
        add     %ecx, %edi
        add     %ecx, %ebx

        clc
        pushf
.loop:
        dec     %esi
        dec     %edi
        dec     %ebx
        mov     (%esi), %al
        popf
        adc     (%edi), %al
        pushf
        mov     %al, (%ebx)

        dec     %ecx
        jnz     .loop

        
        xor     %eax, %eax
        popf
        jnc     .return

.overflow:
        mov     $1, %eax

.return:
        mov     (%esp), %esi
        mov     4(%esp), %edi
        mov     8(%esp), %ebx
        leave
        ret







