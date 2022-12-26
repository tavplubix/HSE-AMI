
        .text
        .global process, end_process
scanfp: 
        .int    scanf
printfp: 
        .int    printf

process:
        jmp     _process
.get_ip:
        mov     (%esp), %edx
        ret

_process:
        call    .get_ip
        push    %ebp
        mov     %esp, %ebp
        add     $-28, %esp
        mov     %ebx, 12(%esp)
        mov     %esi, 20(%esp)
        mov     %edi, 24(%esp)
        
        mov     %edx, %edi

        xor     %ebx, %ebx
        movw    $0x6425, 16(%esp)
        xor     %al, %al
        mov     %al, 18(%esp)
        lea     16(%esp), %eax
        mov     %eax, (%esp)
        lea     8(%esp), %eax
        mov     %eax, 4(%esp)

        mov     %edi, %esi
        add     $-19, %esi 
        mov     (%esi), %esi

.loop:
        call    *%esi
        cmp     $-1, %eax
        je      .end

        mov     8(%esp), %eax
        mov     %eax, %ecx
        and     $0x3, %eax

        shl     $2, %eax
        call    .get_ip
        sub     $-7, %edx
        add     %eax, %edx
        jmp     *%edx

.ST_0:   
        sal     %ecx
        jmp     .ST_D
.ST_1:   
        sar     %ecx
        jmp     .ST_D
.ST_2:
        nop
        inc     %ecx
        jmp     .ST_D
.ST_3:
        dec     %ecx
.ST_D:
        add     %ecx, %ebx
        jmp     .loop

.end:
        movl    $0xFF0A6425, 16(%esp)
        xor     %al, %al
        mov     %al, 19(%esp)
        lea     16(%esp), %edx
        mov     %edx, (%esp)
        mov     %ebx, 4(%esp)
        add     $-15, %edi
        mov     (%edi), %esi
        call    *%esi

        
        mov     12(%esp), %ebx
        mov     20(%esp), %esi
        mov     24(%esp), %edi
        sub     $-28, %esp
        mov     %ebp, %esp
        pop     %ebp
        ret
end_process:

