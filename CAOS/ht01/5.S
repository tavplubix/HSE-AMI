        .data
.previous_rand_result:
        .int    0
.rand_ptr:
        .int    rand
        .text
        .global hijack_rand

hijack_rand:
        enter   $0, $0
        call    rand
        mov     %eax, .previous_rand_result
        mov     $rand, %eax
        movl    2(%eax), %ecx
        movl    (%ecx), %edx
        mov     %edx, .rand_ptr
        movl    $myrand, (%ecx)
        leave
        ret

myrand:
        enter   $0, $0
        mov     .rand_ptr, %eax
        call    *%eax
        xchg    %eax, .previous_rand_result
        and     $0xFFFF, %eax
        leave
        ret

