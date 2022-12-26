        .text 
format:
        .asciz  "%d %d %d\n"
     
        .global print_fib

print_fib:
        push    %ebp
        mov     %esp, %ebp
        
        add     $-12, %esp

        mov     A, %eax
        add     B, %eax
        mov     %eax, (%esp)

        mov     %eax, %ecx
        add     B, %ecx
        mov     %ecx, 4(%esp)
        
        add     %eax, %ecx
        mov     %ecx, 8(%esp)

        push    $format
        call    printf
        
        mov     %ebp, %esp
        pop     %ebp
        ret





