
struct FileReadState
{
    int fd;
    unsigned char *buf;
    int buf_size;
    int buf_capacity;
    int buf_pos;
    int lc;
};

enum { BUF_SIZE = 4096 };

static unsigned char buffer[BUF_SIZE];

static struct FileReadState stdin_state = {0, buffer, 0, BUF_SIZE, 0, 0};

struct FileReadState *stin = &stdin_state;

static int read_to_buffer()
{
    volatile int bytes_read;
    __asm__ volatile    (
                        "mov    $3, %%eax       \n"
                        "mov    (%1), %%ebx     \n"
                        "mov    4(%1), %%ecx    \n"
                        "mov    12(%1), %%edx   \n"
                        "int    $0x80           \n"
                        "mov    %%eax, %0       \n"
                        :
                        "=g" (bytes_read)
                        :
                        "r" (stin)
                        :
                        "eax", "ebx", "ecx", "edx"
                        );
    return bytes_read;
}

int
nextchar()
{
    if (stin->lc < 0) {
        return -1;
    }
    if (stin->buf_size <= stin->buf_pos) {
        int bytes_read = read_to_buffer(stin->buf, stin->buf_capacity);
        if (bytes_read <= 0) {
            stin->buf_size = 0;
            return stin->lc = -1;
        }
        stin->buf_size = bytes_read;
        stin->buf_pos = 0;
    }
    return stin->lc = stin->buf[stin->buf_pos++];

}


int
lastchar()
{
    return stin->lc;
}




