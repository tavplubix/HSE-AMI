#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


int
main()
{
    int pipefd[2];
    pipe(pipefd);
    pid_t child = fork();
    if (child == 0) {
        close(pipefd[1]);
        pid_t grandchild = fork();
        if (grandchild == 0) {
            int64_t sum = 0;
            int32_t i;
            while (read(pipefd[0], &i, sizeof(int32_t)) > 0) {
                sum += i;
            }
            close(pipefd[0]);
            printf("%"PRId64"\n", sum);
            return 0;
        }
        close(pipefd[0]);
        waitpid(grandchild, NULL, 0);
        return 0;
    }
    close(pipefd[0]);

    int32_t i;
    while (scanf("%"SCNd32, &i) != EOF) {
        write(pipefd[1], &i, sizeof(int32_t));
    }

    close(pipefd[1]);
    waitpid(child, NULL, 0);
}

