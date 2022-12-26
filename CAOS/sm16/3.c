#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int
main()
{
    int N;
    scanf("%d", &N);
    pid_t child;
    int id = 1;
    while (id < N) {
        printf("%d ", id);
        fflush(stdout);
        child = fork();
        if (child != 0) {
            break;
        }
        ++id;
    }
    if (id == N) {
        printf("%d\n", id);
    } else {
        waitpid(child, NULL, 0);
    }
}

