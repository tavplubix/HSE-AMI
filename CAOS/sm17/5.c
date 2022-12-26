#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static char **
parse_args(const char *cmd)
{
    enum state { SPACE, SYMBOL, SQUOTE, DQUOTE};
    int argc = 0;
    size_t cmdlen = strlen(cmd) + 1;
    char **argv = malloc((cmdlen / 2 + 1) * sizeof(char *));
    char *buf = malloc(cmdlen * sizeof(char *));
    size_t arg_len = 0;
    enum state state = SPACE;

    while (*cmd) {
        switch(state) {
        case SPACE:
            if (*cmd == '\'') {
                state = SQUOTE;
            } else if (*cmd == '"') {
                state = DQUOTE;
            } else if (*cmd == '\\') {
                buf[arg_len++] = *++cmd;
            } else if (!isspace(*cmd)) {
                state = SYMBOL;
                buf[arg_len++] = *cmd;
            }
            break;
        case SYMBOL:
            if (isspace(*cmd)) {
                buf[arg_len++] = '\0';
                argv[argc] = malloc(arg_len * sizeof(char *));
                strcpy(argv[argc++], buf);
                arg_len = 0;
                state = SPACE;
            } else if (*cmd == '\\') {
                buf[arg_len++] = *++cmd;
            } else if (*cmd == '\'') {
                state = SQUOTE;
            } else if (*cmd == '"') {
                state = DQUOTE;
            } else {
                buf[arg_len++] = *cmd;
            }
            break;
        case SQUOTE:
            if (*cmd == '\'') {
                state = SYMBOL;
            } else if (*cmd == '\\') {
                buf[arg_len++] = *++cmd;
            } else {
                buf[arg_len++] = *cmd;
            }
            break;
        case DQUOTE:
            if (*cmd == '"') {
                state = SYMBOL;
            } else if (*cmd == '\\') {
                buf[arg_len++] = *++cmd;
            } else {
                buf[arg_len++] = *cmd;
            }
            break;
                 
        }
        ++cmd;
    }
    if (state == SYMBOL) {
        buf[arg_len++] = '\0';
        argv[argc] = malloc(arg_len * sizeof(char *));
        strcpy(argv[argc++], buf);
    }
    argv[argc] = NULL;

    free(buf);
    return argv;
}



int
mysystem(const char *cmd)
{
    char **argv = parse_args(cmd);
    if (argv[0] == NULL) {
        return -1;
    }

    int exitcode = 1;
    pid_t child = fork();
    if (child < 0) {
        exitcode = -1;
    } else if (child == 0) {
        execvp(argv[0], argv);
        _exit(1);
    } else {
        int status;
        waitpid(child, &status, 0);
        if (WIFEXITED(status)) {
            exitcode = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            enum { SIGNALED_RET_VAL = 1024 };
            exitcode = SIGNALED_RET_VAL + WTERMSIG(status);
        }
    }

    int i = 0;
    while (argv[i]) {
        free(argv[i++]);
    }
    free(argv);

    return exitcode;
}

