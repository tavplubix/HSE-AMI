        .text
        .global dot_product

dot_product:
        enter   $0, $0
        and     $0xFFFFFFF0, %esp
        add     $-60, %esp
        mov     %esi, 4(%esp)
        mov     %edi, 8(%esp)
        movapd  %xmm0, 12(%esp)
        movapd  %xmm1, 28(%esp)
        movapd  %xmm2, 44(%esp)


        mov     8(%ebp), %eax
        mov     12(%ebp), %esi
        mov     16(%ebp), %edi
        and     $0xFFFFFFFC, %eax
        xor     %ecx, %ecx
        pxor    %xmm0, %xmm0

.loop_packed:
        cmp     %eax, %ecx
        jge     .end_of_loop_packed
        lea     (%esi, %ecx, 4), %edx
        movups  (%edx), %xmm1
        lea     (%edi, %ecx, 4), %edx
        movups  (%edx), %xmm2
        
        mulps   %xmm2, %xmm1
        haddps  %xmm1, %xmm0
        
        add     $4, %ecx
        jmp     .loop_packed

.end_of_loop_packed:
        pxor    %xmm1, %xmm1
        haddps  %xmm1, %xmm0
        haddps  %xmm1, %xmm0

        mov     8(%ebp), %eax
        mov     %eax, %ecx
        and     $0xFFFFFFFC, %ecx

.loop_single:
        cmp     %eax, %ecx
        je      .end_of_loop_single
        lea     (%esi, %ecx, 4), %edx
        movss   (%edx), %xmm1
        lea     (%edi, %ecx, 4), %edx
        movss   (%edx), %xmm2
        
        mulss   %xmm2, %xmm1
        addss   %xmm1, %xmm0

        inc     %ecx
        jmp     .loop_single

.end_of_loop_single:
        movss   %xmm0, (%esp)
        flds    (%esp)

        mov     4(%esp), %esi
        mov     8(%esp), %edi
        movapd  12(%esp), %xmm0
        movapd  28(%esp), %xmm1
        movapd  44(%esp), %xmm2
        leave
        ret






