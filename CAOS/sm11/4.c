
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


__attribute__ ((fastcall))
void 
flush(struct FileWriteState *out)
{
    write(out->fd, out->buf, out->buf_pos);
}

__attribute__ ((fastcall))
void
writechar(int c, struct FileWriteState *out)
{
    if (out->buf_pos == out->buf_size) {
        flush(out);
        out->buf_pos = 0;
    }
    out->buf[out->buf_pos++] = c;
}












