        .text
        .global list_del

list_del:
        enter   $16, $0
        mov     %esi, (%esp)
        mov     %edi, 4(%esp)

        mov     8(%ebp), %esi
        mov     12(%ebp), %eax

        lea     8(%esp), %edi

.loop:
        test    $-1, %esi
        jz      .return
        
        movsxb  (%esi), %ecx
        mov     %esi, %edx
        mov     4(%esi), %esi
        cmp     %eax, %ecx
        je      .loop
        
        mov     %edx, 4(%edi)
        mov     %edx, %edi
        jmp     .loop

.return:
        movl    $0, 4(%edi)    
        mov     12(%esp), %eax

        mov     (%esp), %esi
        mov     4(%esp), %edi
        leave
        ret






