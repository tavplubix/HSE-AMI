#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

int
main()
{
    pid_t pid;
    scanf("%d", &pid);
    printf("%d\n", kill(pid, SIGINT));
    printf("%d\n", kill(pid, SIGINT));
    printf("%d\n", kill(pid, SIGINT));
    printf("%d\n", kill(pid, SIGINT));
    printf("%d\n", kill(pid, SIGINT));
    
}

