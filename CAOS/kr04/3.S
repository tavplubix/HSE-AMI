        .text
unit:   .double 1
        .global myexp

myexp:
        enter   $96, $0
        movupd  %xmm0, (%esp)
        movupd  %xmm1, 16(%esp)
        movupd  %xmm2, 32(%esp)
        movupd  %xmm3, 48(%esp)
        movupd  %xmm4, 64(%esp)
        movupd  %xmm5, 80(%esp)
        
        movsd   unit, %xmm0     // e^x
        xorpd   %xmm1, %xmm1    // n
        movsd   %xmm0, %xmm2    // 1
        movsd   8(%ebp), %xmm3  // x
        movsd   %xmm0, %xmm4    // x^n / n!

.loop:  
        mulsd   %xmm3, %xmm4
        addsd   %xmm2, %xmm1
        divsd   %xmm1, %xmm4

        movsd   %xmm0, %xmm5
        addsd   %xmm4, %xmm0
        
        ucomisd %xmm5, %xmm0
        jne     .loop

        mov     16(%ebp), %eax
        movsd   %xmm0, (%eax)

        
        
        movupd  (%esp), %xmm0
        movupd  16(%esp), %xmm1
        movupd  32(%esp), %xmm2
        movupd  48(%esp), %xmm3
        movupd  64(%esp), %xmm4
        movupd  80(%esp), %xmm5

        leave
        ret





