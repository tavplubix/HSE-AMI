        .text
        .global process

process:
        enter   $12, $0
        mov     %esi, (%esp)
        mov     %edi, 4(%esp)
        mov     %ebx, 8(%esp)

        mov     8(%ebp), %ebx
        
        mov     4(%ebx), %eax
        mov     8(%ebx), %ecx
        imul    %ecx
        mov     %eax, %esi
        mov     %edx, %edi

        mov     (%ebx), %eax
        mov     12(%ebx), %ecx
        imul    %ecx

        sub     %esi, %eax
        sbb     %edi, %edx
        jno     .return


.overflow:
        mov     $0xffffffff, %eax
        mov     %eax, %edx

.return:

        mov     (%esp), %esi
        mov     4(%esp), %edi
        mov     8(%esp), %ebx
        leave
        ret




