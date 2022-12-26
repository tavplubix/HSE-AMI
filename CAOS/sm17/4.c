#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char script1[] =  "#!/bin/python3\n"
                        "import os\n"
                        "print("; //num1*num2*num3*...
const char script2[] =                                  ")\n"
                        "os.remove(os.path.realpath(__file__))\n";

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
    pid_t pid = getpid();
    char tmp_file_name[PATH_MAX];

    const char *tmp_path;
    if ((tmp_path = getenv("XDG_RUNTIME_DIR")) == NULL
        && (tmp_path = getenv("TMPDIR")) == NULL) {
        tmp_path = "/tmp";
    }

    snprintf(tmp_file_name, PATH_MAX, "%s/sm17-4_%d", tmp_path, pid);
    // tmp_file_name is unique for all copies of this program
    int fd = open(tmp_file_name, O_WRONLY | O_TRUNC | O_CREAT, 
                                    S_IRUSR | S_IWUSR | S_IXUSR);
    writeall(fd, script1, sizeof(script1) - 1);
    
    writeall(fd, argv[1], strlen(argv[1]));
    for (int i = 2; i < argc; ++i) {
        writeall(fd, "*", 1);
        writeall(fd, argv[i], strlen(argv[i]));
    }
    writeall(fd, script2, sizeof(script2) - 1);
    
    close(fd);
    execlp(tmp_file_name, tmp_file_name, NULL);
}

