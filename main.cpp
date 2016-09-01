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
#define SERV_PORT  6339
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  int server_sockfd;
  struct sockaddr_in server_sockaddr; // server's address information


  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(MYPORT);
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(server_sockaddr.sin_zero), 8);

  if ((server_sockfd  = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      fprintf(stderr, "socket get error... \n"); exit(1);
  }

  if (bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr)) == -1) {
    fprintf(stderr, "bind error...\n"); exit(1);
  }



}
