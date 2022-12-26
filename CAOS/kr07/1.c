#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

const char prog1[] =    "#include <stdio.h>\n"
                        "#include <math.h>\n"
                        "long double func(long double z) {\n"
                        "   return ";
const char prog2[] =                                    ";\n"
                        "}\n"
                        "int main() {\n"
                        "   long double z;\n"
                        "   while(scanf(\"%Lf\", &z) != EOF) {\n"
                        "       printf(\"%.15Lg\\n\", func(z));\n"
                        "   }\n"
                        "}\n";

static size_t
writeall(int fd, const char *buf, size_t buf_size)
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
    int fd = open("temp.c", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    writeall(fd, prog1, sizeof(prog1) - 1);
    writeall(fd, argv[1], strlen(argv[1]));
    writeall(fd, prog2, sizeof(prog2) - 1);
    close(fd);
    pid_t gcc = fork();
    if (gcc == 0) {
        execlp("gcc", "gcc", "-g", "-lm", "temp.c", "-o", "temp.out", NULL);
        _exit(1);
    }
    waitpid(gcc, NULL, 0);
    unlink("temp.c");
    pid_t child = fork();
    if (child == 0) {
        execlp("./temp.out", "./temp.out", NULL);
        _exit(1);
    }
    waitpid(child, NULL, 0);
    unlink("temp.out");
}

