/* Creates a datagram UDP server.  The port number is passed as an argument. */
/* This server runs forever */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
   int sock, length, n;
   struct sockaddr_in server, from;
   socklen_t fromlen;
   char buf[1024];

   if(argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(1);
   }
   
   sock = socket(AF_INET, SOCK_DGRAM, 0);  /* create new socket */

   if(sock < 0) error("Opening socket");

   length = sizeof(server);
   bzero(&server, length);
   server.sin_family = AF_INET;  /* Internet */
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(atoi(argv[1])); /* port */

   if (bind(sock, (struct sockaddr *)&server, length) < 0) /* bind socket with port */
       error("binding");

   fromlen = sizeof(struct sockaddr_in);

   while (1) {
       n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
       if(n < 0) error("recvfrom");

       printf("Received a datagram: %s\n", buf);
       if(buf[0] == 'X')
           break;

       printf("Please enter the message: ");
       bzero(buf, 1024);
       if (fgets(buf, 1024, stdin) == NULL) {
           break;
       }

       printf("Sent %s\n", buf);
       n = sendto(sock, buf, strlen(buf) + 1, 0,
                  (struct sockaddr *)&from, fromlen);
       if(n < 0) error("sendto");
       if(buf[0] == 'X')
           break;
   }
   
   close(sock);
   return 0;
}

