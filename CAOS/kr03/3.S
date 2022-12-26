        .text
        .global mystrspn

mystrspn:
        enter   $8, $0
        mov     %esi, (%esp)
        mov     %edi, 4(%esp)

        mov     8(%ebp), %esi
        mov     12(%ebp), %edi

        xor     %ecx, %ecx

.loop_s:
        mov     (%esi), %al
        cmp     $0, %al
        je      .return
        push    %edi
        call    .strfind
        pop     %edi
        cmp     $0, %eax
        je      .return
        inc     %esi
        inc     %ecx
        jmp     .loop_s


.return:
        mov     %ecx, %eax

        mov     (%esp), %esi
        mov     4(%esp), %edi
        leave
        ret

.strfind:
        cmpb    $0, (%edi)
        je      .not_found
        cmp     %al, (%edi)
        je      .found
        inc     %edi
        jmp     .strfind
.found:
        mov     $1, %eax
        ret
.not_found:
        xor     %eax, %eax
        ret








