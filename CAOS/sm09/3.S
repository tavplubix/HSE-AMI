        .text
        .global mystrcmp

mystrcmp:
        mov     4(%esp), %eax
        mov     8(%esp), %ecx
        
        mov     $1, %dl
        dec     %ecx
loop:   
        cmp     $0, %dl
        je      equal 
        mov     (%eax), %dl
        inc     %eax
        inc     %ecx
        cmp     (%ecx), %dl
        je      loop
        
        cmp     (%ecx), %dl
        jb      less
greater:
        mov     $1, %eax
        ret
less:
        mov     $-1, %eax
        ret
equal:
        xor     %eax, %eax
        ret



