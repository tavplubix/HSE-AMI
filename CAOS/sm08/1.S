        .text
        .global main
main:
        call    readi32
        pushl   %eax
        call    readi32
        popl    %ebx
        addl    %ebx, %eax
        call    writei32
        call    nl
        call    finish 

