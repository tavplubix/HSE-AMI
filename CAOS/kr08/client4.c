#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

enum { HOST = 1, SERVICE = 2, KEY = 3 };

int
main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    int err = getaddrinfo(argv[HOST], argv[SERVICE], &hints, &res);
    if (err) {
        return 0;
    }
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) {
        return 0;
    }
    if (connect(sfd, res->ai_addr, res->ai_addrlen)) {
        return 0;
    }

    volatile double t = atoi(argv[SERVICE]); 

    union {
        int i;
        float f;
    }d;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 60000000; ++j) {
            t = sqrt(t);
        }
        d.f = i;
        d.i = htonl(d.i);
        write(sfd, &d, sizeof(d));
    }
    close(sfd);
    printf("%g", t);
    error:
    freeaddrinfo(res);
}

