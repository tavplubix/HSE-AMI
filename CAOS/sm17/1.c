#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


int
main(int argc, char **argv)
{
    enum { EXEC_PATH_IDX = 1, IN_FILE_IDX = 2, OUT_FILE_IDX = 3 };
    int infd = open(argv[IN_FILE_IDX], O_RDONLY);
    int outfd = open(argv[OUT_FILE_IDX], O_WRONLY | O_TRUNC | O_CREAT, 
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    infd = dup2(infd, 0);
    outfd = dup2(outfd, 1);
    pid_t child = fork();
    if (child == 0) {
        execlp(argv[EXEC_PATH_IDX], argv[EXEC_PATH_IDX], NULL);
    } else {
        waitpid(child, NULL, 0);
        close(infd);
        close(outfd);
    }   
}

