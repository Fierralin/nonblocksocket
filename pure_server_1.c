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

int main(void) {
  int listenfd, connectfd;
  struct sockaddr_in ser_addr;
  struct sockaddr_in cli_addr;
  socklen_t sin_size;

  ser_addr.sin_family = PF_INET;
  ser_addr.sin_port = SERV_PORT;
  ser_addr.sin_addr.s_addr = INADDR_ANY;

  listenfd  = socket(AF_INET, SOCK_STREAM, 0);
  bind(listenfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
  listen(listenfd, 2);

  connectfd = accept(listenfd, (struct sockaddr *)&cli_addr, &sin_size);
  char sendbuff[] = "ahahaha000000000000000000000000000000000000000000000000000"
  "0000000000000000000000000000000000000000000000000000000000000000000000000000"
  "0000000000000000000000000000000000";
  write(connectfd, sendbuff, strlen(sendbuff));
}
