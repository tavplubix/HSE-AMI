        .data
        .comm   P, 4
        .comm   Q, 4
        .comm   R, 4
        .comm   Z, 4
        
        .text
        .global process

process:
        enter   $4, $0
        mov     %ebx, (%esp)

        mov     P, %eax
        mov     Z, %ecx
        mul     %ecx
        mul     %ecx
        mul     %ecx
        mov     %eax, %ebx

        mov     Q, %eax
        mul     %ecx
        add     %eax, %ebx

        add     R, %ebx

        mov     %ebx, %eax

        mov     (%esp), %ebx
        leave
        ret






