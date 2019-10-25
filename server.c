#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXBUF 1024

int main() {
  int sockfd;
  char buffer[MAXBUF];
  char hello[64];

  struct sockaddr_in servaddr, cliaddr;

  //Creating socket descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  //Whipping possible existing data in structs
  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  //Setting up server parameters
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  //Binding socket and server struct
  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  int p = 35166;
  for (;;){
    printf("Hi !\n" );
    //Rcving msgS
    int len, in;
    in = recvfrom(sockfd, (char *)buffer, MAXBUF, MSG_WAITALL,
                 (struct sockaddr *)&cliaddr, (unsigned int * restrict)&len);
    buffer[in] = '\0';
    printf("Client a recu %d octets: %s \n", in, buffer);

    if (!strcmp("SYN", buffer)){
      printf("Got SYN\n");
      sprintf(hello, "SYN-ACK %d", p);
      sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *)&cliaddr, len);
      printf("SYN-ACK sent \n");
      in = recvfrom(sockfd, (char *)buffer, MAXBUF, MSG_WAITALL,
                   (struct sockaddr *)&cliaddr, (unsigned int * restrict)&len);
      buffer[in] = '\0';
      printf("Client : %s \n", buffer);
      if (!strcmp("ACK", buffer)){
        printf("Got ACK\n");
        int process = fork();
        if (!process) { //Process père
          p ++;
        }else if(process > 0){ //process fils

            if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
              perror("Socket creation failed");
              exit(EXIT_FAILURE);
            }

            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = INADDR_ANY;
            servaddr.sin_port = htons(p);

            if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
              perror("bind client failed");
              exit(EXIT_FAILURE);
            }

            printf("Chld recieving\n");

            in = recvfrom(sockfd, (char *)buffer, MAXBUF, MSG_WAITALL,
                         (struct sockaddr *)&cliaddr, (unsigned int * restrict)&len);
            buffer[in] = '\0';
            printf("Client 3: %s \n", buffer);
            close(sockfd);

        }else{ //erreur
          perror("Erreur lors de la création du fils");
        }
      }
    }

  }
}
