#include <stdio.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int
main()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    int num;
    bool main_process = true;
    pid_t child = 0;
    while (scanf("%d", &num) != EOF) {
        child = fork();
        if (child < 0) {
            if (main_process) {
                printf("-1\n");
                return 0;
            } else {
                return 1;
            }
        }
        if (child) {
            int status;
            waitpid(child, &status, 0);
            if (WEXITSTATUS(status) == 1) {
                if (main_process) {
                    printf("-1\n");
                    return 0;
                } else {
                    return 1;
                }
            }
            printf("%d\n", num);
            return 0;
        } else {
            main_process = false;
        }
    }
}

