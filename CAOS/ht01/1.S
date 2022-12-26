
        .text
        .global process

process:
        enter   $44, $0
        movupd  %xmm0, (%esp)
        movupd  %xmm1, 16(%esp)
        mov     %ebx, 32(%esp)

        call    .ip
        add     $_GLOBAL_OFFSET_TABLE_, %edx
        mov     A@GOT(%edx), %eax
        mov     B@GOT(%edx), %ebx
        mov     C@GOT(%edx), %ecx
        
        movsd   (%eax), %xmm0
        movsd   (%ebx), %xmm1
        
        addsd   %xmm1, %xmm0
        movl    $0, 36(%esp)
        movl    $0x3fe00000, 40(%esp)
        movsd   36(%esp), %xmm1
        mulsd   %xmm1, %xmm0

        movsd   %xmm0, (%ecx)

        mov     32(%esp), %ebx
        movupd  (%esp), %xmm0
        movupd  16(%esp), %xmm1
        leave
        ret

.ip:
        mov     (%esp), %edx
        ret


