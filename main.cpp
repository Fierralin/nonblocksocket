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
#include <pthread.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6339
#define LEN_QUEUE 20
#define BUFFER_SIZE 1024

#define MAXCORES 16

struct ThreadRec {
  int socket_in; // 1, in; 0, none; 2, out
  pthread_t pid;

} ;

void init_ThreadRec();

void runthread(void *arg);

int main(int argc, char *argv[]) {
  int num_cores, corecount;
  struct ThreadRec pthpool[MAXCORES]; // threads

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

  num_cores = 0;
  corecount = 0;
  if (num_cores < MAXCORES) {
    pthread_create(&pthpool[num_cores].pid, NULL, func, (void *)pthpool[num_cores]);

    num_cores++;
  }
  else {

  }

}

void runthread(void *arg) {
  struct ThreadRec *rec = (struct ThreadRec *)arg; //
  int client_sockfd;
  struct sockaddr client_sockaddr;
  while (1) {

  }
}
