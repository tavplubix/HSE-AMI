        .text
        .global main
main:
        call    readi32
loop:
        cmp     $0, %eax
        jl      exit
        push    %eax
        call    writei32
        call    nl
        pop     %eax
        dec     %eax
        jmp     loop

exit:
        call    finish
           


