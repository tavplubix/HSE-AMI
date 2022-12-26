
        .text
.scanf_format:
        .asciz  "%d"
.printf_format:
        .asciz  "%d\n"
.switch_tab:
        .int    .ST_9, .ST_10
        
        .global process

process:
        enter   $16, $0
        mov     %ebx, 12(%esp)

        xor     %ebx, %ebx
        movl    $.scanf_format, (%esp)
        lea     8(%esp), %eax
        mov     %eax, 4(%esp)

.loop:
        call    scanf
        cmp     $-1, %eax
        je      .end

        mov     8(%esp), %eax
        mov     %eax, %ecx
        and     $0xF, %eax

        cmp     $9, %eax
        jl      .ST_D
        cmp     $10, %eax
        jg      .ST_D
        sub     $9, %eax
        jmp     *.switch_tab(, %eax, 4)

.ST_9:   
        inc     %ecx
        jmp     .ST_D
.ST_10:   
        add     %ecx, %ecx
.ST_D:
        add     %ecx, %ebx
        jmp     .loop

.end:
        movl    $.printf_format, (%esp)
        mov     %ebx, 4(%esp)
        call    printf

        
        mov     12(%esp), %ebx
        leave
        ret


