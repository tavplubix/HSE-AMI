        .text
        .global main
main:
        xor     %eax, %eax
        push    $0
loop:
        pop     %ebx
        add     %eax, %ebx
        jo      overflow
        push    %ebx
        call    readi32
        jnc     loop

        add     $4, %esp
        mov     %ebx, %eax
        call    writei32
        call    nl
        call    finish
        
overflow:
        mov     $0x7FFFFFFF, %eax
        adc     $0, %eax
        call    writei32
        call    nl
        call    finish




