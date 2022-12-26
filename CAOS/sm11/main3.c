#include "3.c"


void
_start()
{
    volatile char buf[64];
    for (int i = 0; i < 63; ++i) {
        volatile char c = lastchar();
        buf[i] = nextchar();
        c = lastchar();
    }
    __asm__ volatile    (
                        "mov    $1, %%eax\n"
                        "xor    %%ebx, %%ebx\n"
                        "int    $0x80\n"
                        :
                        :
                        :
                        "eax", "ebx"
                        );
}





