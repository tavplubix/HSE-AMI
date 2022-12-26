        .text
format:
        .asciz  "%llu\n"
        
        .global process

process:
        enter   $28, $0
        movq    %mm0, 20(%esp)
        movq    %mm1, 12(%esp)
        
        movq    A, %mm0
        movq    B, %mm1

        paddq   %mm0, %mm0
        psrlq   $1, %mm1
        psubq   %mm1, %mm0
        
        movq    %mm0, 4(%esp)
        movl    $format, (%esp)

        call    printf

        movq    12(%esp), %mm1
        movq    20(%esp), %mm0
        leave
        ret

