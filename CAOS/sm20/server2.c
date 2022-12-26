#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

int main() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo *res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    int ret = getaddrinfo("localhost", "1337", &hints, &res);
    ret = bind(sfd, res->ai_addr, res->ai_addrlen);
    ret = listen(sfd, 10);
    int fd = accept(sfd, NULL, NULL);
    char buf[1024];
    FILE *fin = fdopen(fd, "r");
    FILE *fout = fdopen(fd, "w");
    setbuf(fin, NULL);
    setbuf(fout, NULL);
    ret = fscanf(fin, "%s", buf);
    ret = printf("%s\n", buf);
    int K;
    ret = scanf("%d", &K);
    ret = fprintf(fout, "%d\n", K);
    for (int i = 0; i <= K; ++i) {
        int t;
        ret = fscanf(fin, "%d", &t);
        ret = printf("%d\n", t);
    }
    uint64_t n;
    scanf("%"SCNu64, &n);
    fprintf(fout, "%"SCNu64, n);
}


