        .text
        .global satsum

satsum:
        mov     4(%esp), %eax
        add     8(%esp), %eax
        jnc     no_carry
        mov     $0xFFFFFFFF, %eax
no_carry:
        ret





