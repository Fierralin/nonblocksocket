#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6339
#define LEN_QUEUE 20
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  int server_sockfd;
  int flag;
  struct sockaddr_in server_sockaddr; // server's address information

  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(SERV_PORT);
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(server_sockaddr.sin_zero), 8);

  if ((server_sockfd  = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "socket get failed ...\n"); exit(1);
  }

  if (bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) == -1) {
    fprintf(stderr, "bind failed ...\n"); exit(1);
  } fprintf(stdout, "socket bind success! \n");

  if (listen(server_sockfd, LEN_QUEUE) == -1) {
    fprintf(stderr, "listen failed ...\n"); exit(1);
  }

  flag = fcntl(server_sockfd, F_SETFL, 0);
  fcntl(server_sockfd, F_GETFL, flag | O_NONBLOCK); // set to non-blocking mode



}
