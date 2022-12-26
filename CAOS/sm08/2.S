        .comm   A, 4
        .comm   B, 4
        .comm   C, 4
        .comm   D, 4
        .comm   R, 4

        .text
        .global process
process:
        movl    A, %eax
        mull    B
        movl    %eax, R
        movl    C, %eax
        mull    D
        subl    %eax, R
        ret
       
        
