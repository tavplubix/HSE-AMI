#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

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
    FILE *sockin = fdopen(sfd, "r");
    FILE *sockout = fdopen(sfd, "w");
    if (sockin == NULL || sockout == NULL) {
        return 0;
    }
    setbuf(sockin, NULL);
    setbuf(sockout, NULL);
    if (fprintf(sockout, "%s\n", argv[KEY]) < 0) {
        goto error;
    }
    int K = 0;
    if (fscanf(sockin, "%d", &K) == EOF) {
        goto error;
    }
    for (int i = 0; i <= K; ++i) {
        if (fprintf(sockout, "%d\n", i) < 0) {
            goto error;
        }
    }
    uint64_t n;
    if (fscanf(sockin, "%"SCNu64, &n) == EOF) {
        goto error;
    }
    printf("%"PRIu64"\n", n);

    error:
    fclose(sockin);
    fclose(sockout);
    freeaddrinfo(res);
}

