#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int
main(int argc, char **argv) 
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int sockopt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons((uint16_t) atoi(argv[1]));
    bind(sfd, (void *) &addr, sizeof(addr));
    listen(sfd, 10);
    int32_t sum = 0;
    int32_t n;
    do {
        int fd = accept(sfd, NULL, NULL);
        read(fd, &n, sizeof(n));
        sum += ntohl(n);
        close(fd);
    } while (n);
    printf("%"PRId32"\n", sum);
    close(sfd);
}


