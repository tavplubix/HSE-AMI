        .text
scanf_format:
        .asciz  "%d%d%d%d"
printf_format:
        .asciz  "%d\n"
        
        .global process

process:
        push    %ebp
        push    %ebx
        mov     %esp, %ebp
        
        add     $-40, %esp
        movl    $0, 36(%esp)

        mov     %esp, %eax
        movl    $scanf_format, (%esp)
        add     $20, %eax
        mov     %eax, 4(%esp)
        add     $4, %eax
        mov     %eax, 8(%esp)
        add     $4, %eax
        mov     %eax, 12(%esp)
        add     $4, %eax
        mov     %eax, 16(%esp)

loop:   
        call    scanf
        cmp     $0, %eax
        jle     eof
        mov     20(%esp), %eax
        mov     24(%esp), %ebx
        mov     28(%esp), %ecx
        mov     32(%esp), %edx

        call    lenght

        add     %eax, 36(%esp)
        jmp     loop

eof:
        mov     36(%esp), %eax
        push    %eax
        push    $printf_format
        call    printf

        mov     %ebp, %esp
        pop     %ebx
        pop     %ebp
        ret


lenght:
        cmp     %eax, %ecx
        jg      ordered
        xchg    %eax, %ecx
        xchg    %ebx, %edx
ordered:
        cmp     %ebx, %edx
        jle     inside
        cmp     %ebx, %ecx
        jl      crossed
        xor     %eax, %eax
        ret
crossed:
        sub     %ecx, %ebx
        mov     %ebx, %eax
        ret
inside:
        sub     %ecx, %edx
        mov     %edx, %eax
        ret


