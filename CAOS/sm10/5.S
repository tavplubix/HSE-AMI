        .text
.format64:
        .asciz  "%llx\n"
.format128:
        .asciz  "%llx%016llx\n"
        .global mul64p

mul64p:
        enter   $16, $0     

        mov     8(%ebp), %eax
        mov     16(%ebp), %ecx

        mul     %ecx
        mov     %eax, 8(%esp)
        mov     %edx, 12(%esp)

        mov     12(%ebp), %eax
        mul     %ecx
        mov     %edx, (%esp)
        add     %eax, 12(%esp)
        adc     $0, (%esp)

        mov     8(%ebp), %eax
        mov     20(%ebp), %ecx
       
        movl    $0, 4(%esp) 
        mul     %ecx
        add     %eax, 12(%esp)
        adc     %edx, (%esp)
        adc     $0, 4(%esp)

        mov     12(%ebp), %eax
        mul     %ecx
        add     %edx, 4(%esp)
        add     %eax, (%esp)
        adc     $0, 4(%esp)

        mov     (%esp), %eax
        or      4(%esp), %eax
        test    $-1, %eax
        jz      .print_64
.print_128:
        push    $.format128
        call    printf
        jmp     .return
.print_64:
        add     $4, %esp
        movl    $.format64, (%esp) 
        call    printf
.return:
        leave
        ret





