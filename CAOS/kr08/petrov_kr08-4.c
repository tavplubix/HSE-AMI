#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static volatile sig_atomic_t term = 1;

void handler(int signum) {
    term = 0;
}

int main(int argc, char *argv[])
{
    struct sigaction sa = {};
    //sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    sigaction(SIGTERM, &sa, NULL);

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
    struct sockaddr_in s1;
    s1.sin_family = AF_INET;
    int port;
    scanf("%d", &port);
    s1.sin_port = htons(port);
    s1.sin_addr.s_addr = INADDR_ANY;
    int r = bind(fd, (struct sockaddr*) &s1, sizeof(struct sockaddr_in));
    if (r < 0) {
        perror("bind");
        return 1;
    }
    listen(fd, 5);

    double result = 0;
    while (term) {
        struct sockaddr_in s2;
        socklen_t len = sizeof(s2);
        int f_num = 0;
        double cur_sum = 0;
        int fd2 = accept(fd, (struct sockaddr*)&s2, &len);
        int val;
        while ((read(fd2, &val, sizeof(val)) > 0) && term) {
            val = ntohl(val);
            float fl_val;
            memcpy(&fl_val, &val, sizeof(fl_val));
            ++f_num;
            cur_sum += (double)fl_val;
        }
        if (f_num && term) {
            result += cur_sum * 1.0 / f_num;
        }
        close(fd2);
    }
    printf("%.10g\n", result);
    fflush(stdout);
    close(fd);
}
