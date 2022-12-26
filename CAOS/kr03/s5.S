        .text
        .global process

process:
        enter   $16, $0
        
        mov     8(%ebp), %eax
        mov     12(%ebp), %ecx

        mov     %eax, (%esp)
        mov     %ecx, 4(%esp)
        movl    $4, 8(%esp)
        movl    $.compare, 12(%esp)
        call    qsort

        leave
        ret

.compare:
        enter   $0, $0

        mov     8(%ebp), %eax
        mov     (%eax), %ecx
        call    .bitcount
        mov     %eax, %edx

        mov     12(%ebp), %eax
        mov     (%eax), %ecx
        call    .bitcount

        cmp     %edx, %eax
        je      .cmp_val
        ja      .cmp_l
        jmp     .cmp_g
        
.cmp_val:
        mov     8(%ebp), %eax
        mov     (%eax), %ecx
        mov     12(%ebp), %eax
        mov     (%eax), %edx
        cmp     %edx,   %ecx
        je      .cmp_e
        ja      .cmp_l
        jmp     .cmp_g
        

.cmp_e:
        xor     %eax, %eax
        leave
        ret
.cmp_l:
        mov     $-1, %eax
        leave
        ret
.cmp_g:
        mov     $1, %eax
        leave
        ret


.bitcount:
        push    %edx
        xor     %eax, %eax
.loop:
        mov     %ecx, %edx
        and     $1, %edx
        add     %edx, %eax
        shr     $1, %ecx
        cmp     $0, %ecx
        jnz     .loop

        pop     %edx
        ret



