        .text
        .global _ZN4Base6hijackEv

_ZN4Base6hijackEv:
        enter   $4, $0
       
        mov     8(%ebp), %eax
        mov     (%eax), %eax
        
        cmpl    $myfunc, 8(%eax)
        je      .end

        movl    $16, (%esp)
        call    malloc
        mov     %eax, %edx
          
        mov     8(%ebp), %eax
        mov     (%eax), %eax

        mov     8(%eax), %ecx
        mov     %ecx, 12(%edx)
        mov     (%eax), %ecx
        mov     %ecx, (%edx)
        mov     4(%eax), %ecx
        mov     %ecx, 4(%edx)
        movl    $myfunc, 8(%edx)

        mov     8(%ebp), %eax
        mov     %edx, (%eax)

.end:
        leave
        ret


myfunc:
        enter   $16, $0
        mov     %esi, 8(%esp)
        mov     %edi, 12(%esp)

        mov     8(%ebp), %esi
        mov     4(%esi), %edi
        
        inc     %edi
        mov     %edi, 4(%esi)

        mov     (%esi), %ecx
        mov     %esi, (%esp)
        mov     12(%ecx), %eax
        call    *%eax

        mov     8(%esp), %esi
        mov     12(%esp), %edi
        leave
        ret


my_destructor:
        enter   $16, $0
        mov     %esi, 8(%esp)
        mov     %edi, 12(%esp)

        mov     8(%ebp), %esi
        mov     4(%esi), %edi
        
        inc     %edi
        mov     %edi, 4(%esi)

        mov     (%esi), %ecx
        mov     %esi, (%esp)
        mov     12(%ecx), %eax
        call    *%eax

        mov     8(%esp), %esi
        mov     12(%esp), %edi
        leave
        ret


        
