#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

extern char process[], end_process[];

int main(void)
{
    size_t size = end_process - process;
    if (size < 10 || size > 1024) {
        fprintf(stderr, "invalid code size: %zu\n", size);
        return 1;
    }
    char *ptr = malloc(size + 8);
    if (!ptr) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }
    size_t len = strnlen(process, size);
    memcpy(ptr + 8, process, len);

    unsigned long addr1 = (unsigned long) process - 8;
    unsigned long addr2 = (unsigned long) end_process;
    addr1 = addr1 & -4096;
    addr2 = (addr2 + 4095) & -4096;
    mprotect((void*) addr1, addr2 - addr1, PROT_READ | PROT_WRITE | PROT_EXEC);
    *((void**) process - 1) = NULL;
    *((void**) process - 2) = NULL;
    memset(process, 0, size);

    *((void**) ptr) = scanf;
    *((void**) ptr + 1) = printf;
    /*
    // scanf fixup
    char *newscanp = ptr + (scanp - process);
    unsigned offset = (char*) scanf - (newscanp + 5);
    *((unsigned*) (newscanp + 1)) = offset;

    // printf fixup
    char *newprintp = ptr + (printp - process);
    offset = (char*) printf - (newprintp + 5);
    *((unsigned*) (newprintp + 1)) = offset;
    */
    ((void (*)(void)) (ptr + 8))();

    //((void (*)(void)) process)();

}
