/* Creates datagram UDP client in the internet domain. */
/* IP address and port are passed as argument */
#include <sys/types.h>                       //MANJKA 1
#include <stdlib.h>                          //MANJKA 1
#include <unistd.h>                          //MANJKA 1
#include <sys/socket.h>                      //MANJKA 1
#include <netinet/in.h>                      //MANJKA 1
#include <string.h>                          //MANJKA 1
#include <netdb.h>                           //MANJKA 1
#include <stdio.h>                           //MANJKA 1

void error(const char *msg) {                //MANJKA 2
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
   struct hostent *hp;                       //MANJKA 2
   int sock, length, n;                      //MANJKA 2
   struct sockaddr_in server, from;          //MANJKA 2
   socklen_t fromlen;                        //MANJKA 2
   char buf[1024];                           //MANJKA 2
   
   if(argc != 3) { 
      printf("Usage: client IP port\n");
      exit(1);
   }

   sock = socket(AF_INET, SOCK_DGRAM, 0);

   if(sock < 0) error("socket");

   bzero((char *)&server, sizeof(server));
   server.sin_family = AF_INET;
   hp = gethostbyname(argv[1]);

   if(hp == NULL) error("Unknown host");

   bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
   server.sin_port = htons(atoi(argv[2]));
   length = sizeof(struct sockaddr_in);

   while(1){                                  //MANJKA 3
      printf("Please enter the message: ");
      bzero(buf, sizeof(buf));
      if (fgets(buf, sizeof(buf), stdin) == NULL) {
         break;
      }

      printf("Send %s\n", buf);
      n = sendto(sock, buf, strlen(buf) + 1, 0,
                 (const struct sockaddr *)&server, length);
      if(n < 0) error("sendto");

      if(buf[0] == 'X')                      //MANJKA 3 in 4
        break;

      fromlen = sizeof(struct sockaddr_in);
      n = recvfrom(sock, buf, sizeof(buf), 0,
                   (struct sockaddr *)&from, &fromlen);   //MANJKA 5
      if(n < 0) error("recvfrom");
      printf("Received a datagram: %s\n", buf);

      if(buf[0] == 'X')                      //MANJKA 3 in 4
        break;
   }
   
   close(sock);
   return 0;
}
