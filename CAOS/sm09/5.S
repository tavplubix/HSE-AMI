        .text
scanf_format:
        .asciz  "%d"
printf_format:
        .asciz  "%d "
printf_newline:
        .asciz  "\n"
        
        .global process

process:
        push    %ebp
        push    %ebx
        push    %esi
        mov     %esp, %ebp
        
        add     $-12, %esp
        lea     8(%esp), %eax
        mov     %eax, 4(%esp)
        movl    $scanf_format, (%esp)
        call    scanf
        mov     8(%esp), %ebx
        cmp     $0, %ebx
        je      .end
        mov     %ebx, %esi

.read:   
        call    scanf
        lea     4(%esp), %edx
        mov     %edx, (%esp)
        push    $scanf_format
        dec     %esi
        cmp     $0, %esi
        jnz     .read

        add     $4, %esp
        mov     %ebx, %esi
.write:
        add     $4, %esp
        movl    $printf_format, (%esp)
        call    printf
        dec     %esi
        cmp     $0, %esi
        jnz     .write

        movl    $printf_newline, (%esp)
        call    printf
.end:
        mov     %ebp, %esp
        pop     %esi
        pop     %ebx
        pop     %ebp 
        ret










