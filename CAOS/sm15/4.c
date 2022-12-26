#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


static bool
decode(uint8_t next_byte, int32_t *result)
{
    enum { VAL_BITS_IN_BITE = 7, BUF_SIZE = 5 };
    static uint8_t buf[BUF_SIZE];
    static int n = 0;
    const uint8_t part_mask = 0x7F;
    buf[n++] = next_byte;
    if (!(next_byte & ~part_mask)) {
        if (next_byte & (~part_mask >> 1)) {
            while (n < BUF_SIZE) {
                buf[n++] = part_mask;
            }
        }
        *result = 0;
        const uint64_t mask = 0xFFFFFFFF;
        for (int i = 0; i < n; ++i) {
            uint64_t next_part = (buf[i] & part_mask);
            next_part <<= (i * VAL_BITS_IN_BITE);
            *result |= next_part & mask;
        }
        n = 0;
        return true;
    }
    if (n == BUF_SIZE) {
        _exit(1);
    }
    return false;
}


int
main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    size_t size = st.st_size;
    printf("%zu\n", size);
    if (size == 0) {
        printf("%d\n%d\n", 0, 0);
        return 0;
    }
    size_t beg = 0;
    size_t end = size;
    while (beg != end) {
        size_t mid = (beg + end + 1) / 2;
        void *arr = mmap(NULL, mid, PROT_READ, MAP_PRIVATE, fd, 0);
        if (arr == MAP_FAILED) {
            end = mid - 1;
        } else {
            munmap(arr, mid);
            beg = mid;
        }    
    }
    
    printf("%zu\n", beg);

    size_t pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize < beg) {
        beg = (beg / pagesize) * pagesize;
    }
    
    size_t pages = beg / pagesize;

    uint8_t *arr = NULL;
    size_t pcount = 0;
    int64_t sum = 0;
    for (size_t i = 0; i < size; ++i) {
        if (i % beg == 0) {
            munmap(arr, beg);
            if (pcount == pages) {
                arr = mmap(NULL, end, PROT_READ, MAP_PRIVATE, fd, pcount * beg);
            } else {
                arr = mmap(NULL, beg, PROT_READ, MAP_PRIVATE, fd, pcount * beg);
            }
            ++pcount;
        }
        int32_t next_val;
        if (decode(arr[i - (pcount - 1) * beg], &next_val)) {
            sum += next_val;
        }
    }
    
    printf("%"PRId64"\n", sum);

    munmap(arr, end);
    close(fd);
}

