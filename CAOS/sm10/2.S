        .text        
        .global myhypot

myhypot:
        enter   $8, $0
        movq    %xmm0, (%esp)
        
        movupd  8(%ebp), %xmm0
        mulpd   %xmm0, %xmm0
        haddpd  %xmm0, %xmm0
        sqrtsd  %xmm0, %xmm0
        mov     24(%ebp), %eax
        movsd   %xmm0, (%eax)
        
        movq    (%esp), %mm0
        leave
        ret

