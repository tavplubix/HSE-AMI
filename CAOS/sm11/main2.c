
static const int stdin = 0;
static const int stdout = 1;


static int
read(int fd, char *buffer, int max_size)
{
    volatile int bytes_read;
    __asm__ volatile    (
                        "mov    $3, %%eax   \n"
                        "mov    %1, %%ebx   \n"
                        "mov    %2, %%ecx   \n"
                        "mov    %3, %%edx   \n"
                        "int    $0x80       \n"
                        "mov    %%eax, %0   \n"
                        :
                        "=g" (bytes_read)
                        :
                        "g" (fd), "g" (buffer), "g" (max_size)
                        :
                        "eax", "ebx", "ecx", "edx"
                        );
    return bytes_read;
}

static void
write(int fd, char *buffer, int size)
{
    __asm__ volatile    (
                        "mov    $4, %%eax   \n"
                        "mov    %0, %%ebx   \n"
                        "mov    %1, %%ecx   \n"
                        "mov    %2, %%edx   \n"
                        "int    $0x80       \n"
                        :
                        :
                        "g" (fd), "g" (buffer), "g" (size)
                        :
                        "eax", "ebx", "ecx", "edx"
                        );
}

static void
process(char *str, int size)
{
    for (int i = 0; i < size; ++i) {
        if ('a' <= str[i] && str[i] <= 'z') {
            str[i] += 'A' - 'a';
        }
    }
}

static void
exit(int exit_code)
{
    __asm__ volatile    (
                        "mov    $1, %%eax   \n"
                        "mov    %0, %%ebx   \n"
                        "int    $0x80       \n"
                        :
                        :
                        "g" (exit_code)
                        :
                        "eax", "ebx"
                        );
}

void
_start()
{
    enum { BUF_SIZE = 4096 };
    char buf[BUF_SIZE];
    int size;
    while (0 < (size = read(stdin, buf, BUF_SIZE))) {
        process(buf, size);
        write(stdout, buf, size);
    }
    exit(0);
}





