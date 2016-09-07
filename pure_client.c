#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>

#define LISTENQ 20
#define SERV_PORT 6339
#define SERV_IP "127.0.0.1"

int main(void) {
  struct sockaddr_in cli_addr;
  int sockfd;
  char buff[1024];

  cli_addr.sin_family = AF_INET;
  cli_addr.sin_port = SERV_PORT;
  cli_addr.sin_addr.s_addr = inet_addr(SERV_IP);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  connect(sockfd, (struct sockaddr *)&cli_addr, sizeof(struct sockaddr));
  recv(sockfd, buff, 1024, 0);
  fprintf(stdout, "%s\n", buff);
  close(sockfd);
}
