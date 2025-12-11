/* Asynchronous UDP client: can send and receive at any time. */
/* Usage: ./client2 <server_host> <port> */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sock, n;
    struct sockaddr_in server;
    struct hostent *hp;
    socklen_t serverlen;
    char buf[1024];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_host> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("socket");

    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;

    hp = gethostbyname(argv[1]);
    if (hp == NULL) error("Unknown host");

    bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    server.sin_port = htons(atoi(argv[2]));
    serverlen = sizeof(server);

    printf("Asynchronous UDP client started. Type 'X' to exit.\n");

    while (1) {
        fd_set readfds;
        int maxfd;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            error("select");
        }

        /* Input z tipkovnice -> pošlji serverju */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            bzero(buf, sizeof(buf));
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                /* EOF (npr. Ctrl+D) */
                printf("EOF on stdin, exiting.\n");
                break;
            }

            /* Odstrani \n na koncu (opcijsko) */
            size_t len = strlen(buf);
            if (len > 0 && buf[len - 1] == '\n') {
                buf[len - 1] = '\0';
            }

            n = sendto(sock, buf, strlen(buf) + 1, 0,
                       (struct sockaddr *)&server, serverlen);
            if (n < 0) error("sendto");

            if (buf[0] == 'X') {
                printf("You sent X, exiting.\n");
                break;
            }
        }

        /* Podatki s socket-a -> izpiši sporočilo */
        if (FD_ISSET(sock, &readfds)) {
            bzero(buf, sizeof(buf));
            n = recvfrom(sock, buf, sizeof(buf), 0,
                         (struct sockaddr *)&server, &serverlen);
            if (n < 0) error("recvfrom");

            printf("Server: %s\n", buf);

            if (buf[0] == 'X') {
                printf("Server sent X, exiting.\n");
                break;
            }
        }
    }

    close(sock);
    return 0;
}
