        .text
        .global forward.args

forward.args:
        enter   $16, $0
        
        mov     20(%ebp), %eax
        mov     %eax, (%esp)

        mov     16(%ebp), %eax
        mov     %eax, 4(%esp)

        mov     8(%ebp), %eax
        mov     %eax, 8(%esp)
        mov     12(%ebp), %eax
        mov     %eax, 12(%esp)

        call    process
        
        xor     $0xffffffff, %eax

        leave
        ret






