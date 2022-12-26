#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


static size_t
writeall(int fd, void *buf, size_t buf_size)
{
    size_t total_written = 0;
    ssize_t last_written;
    do {
        last_written = write(fd, buf + total_written, buf_size - total_written);
        if (last_written < 0) {
            return total_written;
        }
        total_written += last_written;
    } while (total_written != buf_size);
    return total_written;
}

int
main(int argc, char **argv)
{
    enum { N_IDX = 1, F_IDX = 2, A0_IDX = 3, D_IDX = 4, K_IDX = 5 };
    int N = atoi(argv[N_IDX]);
    int A0 = atoi(argv[A0_IDX]);
    int D = atoi(argv[D_IDX]);
    int K = atoi(argv[K_IDX]);
    int fd = open(argv[F_IDX], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(fd, sizeof(int) * K * N);
    close(fd);

    int id = 0;
    while (id < N) {
        pid_t child = fork();
        if (child == 0) {
            break;
        }
        ++id;
    }
    if (id < N) {
        int fd = open(argv[F_IDX], O_WRONLY);
        for (int i = 0; i < K; ++i) {
            off_t pos = N * i + id;
            int Ai = A0 + D * pos;
            lseek(fd, pos * sizeof(int), SEEK_SET);
            writeall(fd, &Ai, sizeof(int));
        }
        close(fd);
    } else {
        while (id--) {
            waitpid(-1, NULL, 0);
        }
    }
}

