#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <regex.h>

#define PORT 8080
#define MAXBUF 1024
#define ip "127.0.0.1"

int main(){
  int sockfd;
  char buffer[MAXBUF];
  char *hello = "SYN";

  struct sockaddr_in servaddr;

  //Creating socket descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  //Resetting struct
  memset(&servaddr, 0, sizeof(servaddr));

  //Setting up serv info
  servaddr.sin_family = AF_INET;
  inet_aton(ip, &servaddr.sin_addr);
  servaddr.sin_port = htons(PORT);

  int n, len;

  //SYN
  sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
        (const struct sockaddr *) &servaddr, sizeof(servaddr));

  printf("Message sent\n");
  //SYN-ACK ?
  n = recvfrom(sockfd, (char *)buffer, MAXBUF, MSG_WAITALL ,
              (struct sockaddr *)&servaddr, (unsigned int * restrict)&len);

  buffer[n] = '\0'; //Terminating string
  printf("Server %s\n", buffer);

  //Testing returned value
  regex_t reg;
  int reti;
  size_t maxMatch = 2;
  regmatch_t matches[maxMatch];
  char *rcvport;
  if ( reti = regcomp(&reg, "SYN-ACK ([0-9]{1,5})", REG_EXTENDED) != 0 ){
    perror("Erreur avec la regex");
    exit(EXIT_FAILURE);
  }
  if ( reti = regexec(&reg, buffer, maxMatch, matches, 0) == 0 ){
    printf("Got SIN-ACK\n" );
    // printf("Port : %.*s\n", (int)(matches[1].rm_eo - matches[1].rm_so), buffer + matches[1].rm_so);
  }else{
    fprintf(stderr, "not syn-ack\n");
    exit(EXIT_FAILURE);
  }
  rcvport = malloc(matches[1].rm_eo - matches[1].rm_so + 1); //+1 is for EOstring
  memcpy(rcvport, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
  rcvport[matches[1].rm_eo - matches[1].rm_so] = '\0';

  printf("Port : %s\n", rcvport);

  regfree(&reg);

  //ACK
  hello = "ACK";
  sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
        (const struct sockaddr *) &servaddr, sizeof(servaddr));

  printf("Message sent\n");

  //First message
  servaddr.sin_port = htons(atoi(rcvport));


  hello = "cqsqdq !";
  sleep(1);
  sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
        (const struct sockaddr *) &servaddr, sizeof(servaddr));

  printf("Hi sent\n");

  close(sockfd);
  return 0;
}
