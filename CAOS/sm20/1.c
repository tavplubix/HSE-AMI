#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum { BUF_SIZE = 1024 };

int
main()
{
    char host[BUF_SIZE];
    char service[BUF_SIZE];
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    while (scanf("%s%s", host, service) != EOF) {
        int err = getaddrinfo(host, service, &hints, &res);
        if (err) {
            printf("%s\n", gai_strerror(err));
        } else {
            struct addrinfo *node = res;
            uint32_t minip = UINT32_MAX;
            uint32_t port = 0;
            struct in_addr minaddr;
            do {
                struct sockaddr_in *addr = (void *) node->ai_addr;
                uint32_t ip = ntohl(addr->sin_addr.s_addr);
                if (ip <= minip) {
                    minip = ip;
                    minaddr = addr->sin_addr;
                    port = ntohs(addr->sin_port);
                }
                node = node->ai_next;
            } while (node != NULL);
            printf("%s:%"PRIu16"\n", inet_ntoa(minaddr), port);
            freeaddrinfo(res);
        }
    }
}

