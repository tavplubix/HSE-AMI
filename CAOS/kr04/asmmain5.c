void
_start() {
    __asm__ volatile (
    "        jmp     __start\n"
            ".readfile:\n"
            "        mov     $0x2d, %%eax\n"
            "        xor     %%ebx, %%ebx\n"
            "        int     $0x80\n"
            "        mov     %%eax, %%esi\n"
            "        lea     1(%%eax), %%ebx\n"
            "        mov     $0x2d, %%eax\n"
            "        int     $0x80\n"
            "        movb    $0x0A, -1(%%eax)\n"
            "        mov     %%eax, %%edi\n"

            ".read_loop:\n"
            "        mov     $0x2d, %%eax\n"
            "        mov     %%edi, %%ebx\n"
            "        add     $4096, %%ebx\n"
            "        int     $0x80\n"

            "        mov     $3, %%eax\n"
            "        xor     %%ebx, %%ebx\n"
            "        mov     %%edi, %%ecx\n"
            "        mov     $4096, %%edx\n"
            "        int     $0x80\n"

            "        add     %%eax, %%edi\n"
            "        cmp     $0, %%eax\n"
            "        jg      .read_loop\n"
            "        jl      .error\n"

            "        cmp     %%esi, %%edi\n"
            "        je      .readfile_return\n"
            "        cmpb    $0x0A, -1(%%edi)\n"
            "        je      .readfile_return\n"
            "        cmp     $4096, %%eax\n"
            "        jne     .add_newline\n"

            "        mov     $0x2d, %%eax\n"
            "        lea     1(%%edi), %%ebx\n"
            "        int     $0x80\n"
            ".add_newline:\n"
            "        movb    $0x0A, (%%edi)\n"
            "        inc     %%edi\n"
            ".readfile_return:\n"
            "        inc     %%esi\n"
            "        ret\n"


            ".writeall:\n"
            "        push    %%ecx\n"
            "        push    %%edx\n"
            "        mov     $4, %%eax\n"
            "        mov     $1, %%ebx\n"
            "        int     $0x80\n"
            "        cmp     $0, %%eax\n"
            "        jl      .error\n"

            "        pop     %%edx\n"
            "        pop     %%ecx\n"
            "        add     %%eax, %%ecx\n"
            "        sub     %%eax, %%edx\n"
            "        test    $-1, %%edx\n"
            "        jnz     .writeall\n"

            "        ret\n"


            "__start:\n"
            "        call    .readfile\n"
            "        xor     %%edx, %%edx\n"

            ".print_loop:\n"
            "        cmp     %%esi, %%edi\n"
            "        je      .print_last_string\n"
            "        inc     %%edx\n"
            "        dec     %%edi\n"
            "        cmpb    $0x0A, -1(%%edi)\n"
            "        jne     .print_loop\n"

            "        mov     %%edi, %%ecx\n"
            "        call    .writeall\n"
            "        xor     %%edx, %%edx\n"
            "        jmp     .print_loop\n"


            ".print_last_string:\n"
            "        mov     %%esi, %%ecx\n"
            "        call    .writeall\n"
            "        call    .exit\n"
            "\n"
            "\n"
            ".exit:\n"
            "        xor     %%eax, %%eax\n"
            ".error:\n"
            "        mov     %%eax, %%ebx\n"
            "        mov     $1, %%eax\n"
            "        int     $0x80\n"
    :: :
    );

}
