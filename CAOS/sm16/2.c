#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int
main()
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    enum { BUF_SIZE = 9, CHILDS = 3 };
    pid_t childs[CHILDS];
    int id = 0;
    while (id < CHILDS) {
        childs[id] = fork();
        if (childs[id] == 0) {
            break;
        }
        ++id;
    }
    if (id < CHILDS) {
        char buf[BUF_SIZE];
        fread(buf, sizeof(char), BUF_SIZE - 1, stdin);
        buf[BUF_SIZE - 1] = '\0';
        int32_t num = atol(buf);
        printf("%d %"PRId32"\n", id + 1, num * num);
    } else {
        while (id--) {
            waitpid(-1, NULL, 0);
        }
    }
}


