        .text
.cmp_prec:
        .double 1e-8

        .align  16
.cmp_mask:
        .quad   -1, 0
.sgn_mask:
        .quad   0x7FFFFFFFFFFFFFFF, 0
        .global tree_find

tree_find:
        enter   $0, $0
        and     $0xFFFFFFF0, %esp
        add     $-64, %esp
        movapd  %xmm0, (%esp)
        movapd  %xmm1, 16(%esp)
        movapd  %xmm2, 32(%esp)
        movapd  %xmm3, 48(%esp)

        mov     8(%ebp), %ecx
        movsd   12(%ebp), %xmm0
        movsd   .cmp_prec, %xmm1
.loop:
        cmp     $0, %ecx
        je      .not_found
        movsd   (%ecx), %xmm2
        subsd   %xmm0, %xmm2
        
        // I hope there are no NaNs
        movsd   %xmm2, %xmm3
        andpd   .sgn_mask, %xmm3
        cmplesd %xmm1, %xmm3
        ptest   .cmp_mask, %xmm3
        jnz     .found
        
        pxor    %xmm3, %xmm3
        cmplesd %xmm2, %xmm3
        ptest   .cmp_mask, %xmm3
        jnz     .left
.right:
        mov     12(%ecx), %ecx
        jmp     .loop
.left:
        mov     8(%ecx), %ecx
        jmp     .loop


.not_found:
        xor     %eax, %eax
        jmp     .return
          
.found: 
        mov     $1, %eax
        mov     20(%ebp), %edx
        cmp     $0, %edx
        je      .return
        mov     16(%ecx), %ecx
        mov     %ecx, (%edx)

.return:
        movapd  (%esp), %xmm0
        movapd  16(%esp), %xmm1
        movapd  32(%esp), %xmm2
        movapd  48(%esp), %xmm3
        leave
        ret






