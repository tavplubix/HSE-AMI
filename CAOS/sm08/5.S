        .data
even:   .int    0
odd:    .int    0

        .text
        .global main
main:
        xor     %eax, %eax
loop:
        test    $1, %eax
        jz      add_even
add_odd:
        add     %eax, odd
        jmp     read_next
add_even:
        add     %eax, even
read_next:
        call    readi32
        jnc     loop

exit:
        mov     even, %eax
        call    writei32
        call    nl

        mov     odd, %eax
        call    writei32
        call    nl

        call    finish


