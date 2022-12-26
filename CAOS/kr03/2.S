        .text
        .global process

process:
        enter   $4, $0
        mov     %ebx, (%esp)
        
        mov     8(%ebp), %eax
        mov     $4, %ecx
        imul    %ecx
        jo      .overflow
        mov     %eax, %ebx
        
        mov     12(%ebp), %eax
        mov     $-5, %ecx
        imul    %ecx
        jo      .overflow 
        add     %eax, %ebx
        jo      .overflow
        

        mov     16(%ebp), %eax
        mov     %ebx, (%eax)
        mov     $1, %eax
        jmp     .return

        
.overflow:
        xor     %eax, %eax
.return:
        mov     (%esp), %ebx
        leave
        ret












