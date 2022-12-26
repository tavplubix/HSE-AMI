#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static volatile sig_atomic_t print = 0;

void
handler(int signum) {
    print = 1;
}

static size_t
readall(int fd, void *buf, ssize_t buf_size)
{
    size_t total_read = 0;
    ssize_t last_read;
    do {
        last_read = read(fd, buf + total_read, buf_size - total_read);
        if (last_read <= 0) {
            return total_read;
        }
        total_read += last_read;
    } while (total_read != buf_size);
    return total_read;
}

int
main(int argc, char **argv) 
{
    struct sigaction sigact;
    memset(&sigact, 0, sizeof(sigact));
    sigact.sa_handler = handler;
    sigaction(SIGTERM, &sigact, NULL);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int sockopt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                &sockopt, sizeof(sockopt));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    uint16_t port;
    scanf("%"SCNu16, &port);
    addr.sin_port = htons(port);
    bind(sfd, (void *) &addr, sizeof(addr));
    enum { MAX_QUEUE = 10 };
    listen(sfd, MAX_QUEUE);

    double sum = 0;
    while (print == 0) {
        int fd = accept(sfd, NULL, NULL);
        double client_sum = 0;
        int n = 0;
        union {
            float f;
            uint32_t i;
        } num;
        while (print == 0 
                && readall(fd, &(num.f), sizeof(float)) == sizeof(float)) {
            num.i = ntohl(num.i);
            client_sum += num.f;
            ++n;
        }
        if (n && !print) {
            sum += client_sum / n;
        }
        close(fd);
    }
    printf("%.10g\n", sum);
    close(sfd);
}


