#include <stdio.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
main()
{
    pid_t child = fork();
    if (child == 0) {
        pid_t grandchild = fork();
        if (grandchild == 0) {
            printf("3 ");
            return 0;
        }
        waitpid(grandchild, NULL, 0);
        printf("2 ");
        return 0;
    }
    waitpid(child, NULL, 0);
    printf("1\n");
}


