/* Asynchronous UDP server: can send and receive at any time. */
/* Usage: ./server2 <port> */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
    struct sockaddr_in server, from;
    socklen_t fromlen;
    char buf[1024];
    int client_known = 0;  /* ali že poznamo clienta? */
    struct sockaddr_in client;
    socklen_t clientlen;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("Opening socket");

    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
        error("binding");

    fromlen = sizeof(from);

    printf("Asynchronous UDP server started on port %s.\n", argv[1]);
    printf("Waiting for first client message...\n");
    printf("Type 'X' to exit after a client connects.\n");

    while (1) {
        fd_set readfds;
        int maxfd;

        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        maxfd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) < 0) {
            error("select");
        }

        /* Podatki z omrežja (od clienta) */
        if (FD_ISSET(sock, &readfds)) {
            bzero(buf, sizeof(buf));
            n = recvfrom(sock, buf, sizeof(buf), 0,
                         (struct sockaddr *)&from, &fromlen);
            if (n < 0) error("recvfrom");

            /* Če še nimamo clienta, si ga zapomnimo */
            if (!client_known) {
                client = from;
                clientlen = fromlen;
                client_known = 1;
                printf("Client connected (address stored).\n");
            }

            printf("Client: %s\n", buf);

            if (buf[0] == 'X') {
                printf("Client sent X, exiting.\n");
                break;
            }
        }

        /* Vnos z tipkovnice: pošlji clientu, če ga poznamo */
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            bzero(buf, sizeof(buf));
            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf("EOF on stdin, exiting.\n");
                break;
            }

            /* Odstrani \n */
            size_t len = strlen(buf);
            if (len > 0 && buf[len - 1] == '\n') {
                buf[len - 1] = '\0';
            }

            if (!client_known) {
                printf("No client connected yet, cannot send.\n");
                continue;
            }

            n = sendto(sock, buf, strlen(buf) + 1, 0,
                       (struct sockaddr *)&client, clientlen);
            if (n < 0) error("sendto");

            if (buf[0] == 'X') {
                printf("You sent X, exiting.\n");
                break;
            }
        }
    }

    close(sock);
    return 0;
}
