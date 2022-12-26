#include <bits/fcntl-linux.h.h>

inline static int
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

inline static int
open(char *path, int flags, int mode)
{
    volatile int fd;
    __asm__ volatile    (
            "mov    $5, %%eax   \n"
            "mov    %1, %%ebx   \n"
            "mov    %2, %%ecx   \n"
            "mov    %3, %%edx   \n"
            "int    $0x80       \n"
            "mov    %%eax, %0   \n"
    :
    "=g" (fd)
    :
    "g" (path), "g" (flags), "g" (mode)
    :
    "eax", "ebx", "ecx", "edx"
    );
    return fd;
}

inline static int
write(int fd, char *buffer, int size)
{
    volatile int bytes_read;
    __asm__ volatile    (
                        "mov    $4, %%eax   \n"
                        "mov    %1, %%ebx   \n"
                        "mov    %2, %%ecx   \n"
                        "mov    %3, %%edx   \n"
                        "int    $0x80       \n"
                        "mov    %%eax, %0   \n"
                        :
                        "=g" (bytes_read)
                        :
                        "g" (fd), "g" (buffer), "g" (size)
                        :
                        "eax", "ebx", "ecx", "edx"
                        );
    return bytes_read;
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


static int
readall(int fd, char *buf, int buf_size)
{
    int total_read = 0;
    int last_read;
    do {
        last_read = read(fd, buf + total_read, buf_size - total_read);
        if (last_read <= 0) {
            return total_read;
        }
        total_read += last_read;
    } while (total_read != buf_size);
    return total_read;
}

static int
writeall(int fd, char *buf, int buf_size)
{
    int total_written = 0;
    int last_written;
    do {
        last_written = write(fd, buf + total_written, buf_size - total_written);
        if (last_written < 0) {
            return total_written;
        }
        total_written += last_written;
    } while (total_written != buf_size);
    return total_written;
}


struct FileWriteState
{
    int fd;
    unsigned char *buf;
    int buf_size;
    int buf_pos;
};

enum { BUF_SIZE = 4096 };

static unsigned char buffer[BUF_SIZE];

static struct FileWriteState stdout_state = {1, buffer, BUF_SIZE, 0};

struct FileWriteState *stout = &stdout_state;

inline static void
flush(struct FileWriteState *out)
{
    writeall(out->fd, out->buf, out->buf_pos);
}

inline static void
writechar(int c, struct FileWriteState *out)
{
    if (out->buf_pos == out->buf_size) {
        flush(out);
        out->buf_pos = 0;
    }
    out->buf[out->buf_pos++] = c;
}

static void
writestr(char *str, int size, struct FileWriteState *out)
{
    for (int i = 0; i < size; ++i) {
        writechar(str[i], out);
    }
}


static void *
brk(void *ptr)
{
    volatile void *new_ptr;
    __asm__ volatile    (
                        "mov    $0x2d, %%eax   \n"
                        "mov    %1, %%ebx   \n"
                        "int    $0x80       \n"
                        "mov    %%eax, %0   \n"
                        :
                        "=g" (new_ptr)
                        :
                        "g" (ptr)
                        :
                        "eax", "ebx"
                        );
    return new_ptr;
}

static int
read_file(int fd, char **buf)
{
    int size = 0;
    void *end = brk(0);
    *buf = end = brk(end + BUF_SIZE);
    (*buf)[-1] = '\n';
    int last;
    do {
        end = brk(end + BUF_SIZE);
        last = readall(fd, *buf + size, BUF_SIZE);
        size += last;
    } while (last == BUF_SIZE);
    if (size != 0 && (*buf)[size - 1] != '\n') {
        (*buf)[size++] = '\n';
    }
    return size;
}

void
_start() {
    stout->fd = open("out", O_WRONLY | O_TRUNC, 0777);
    char *beg, *end;
    int size = read_file(0, &beg);
    end = beg + size;
    int strlen = 0;
    while (beg != end) {
        --end;
        ++strlen;
        if (end[-1] == '\n') {
            writeall(1, end, strlen);
            strlen = 0;
        }
    }
    writeall(1, end, strlen);
    exit(0);
}


