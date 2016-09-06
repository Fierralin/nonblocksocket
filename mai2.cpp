#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
#include <errno.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6339
#define LEN_QUEUE 20
#define BUFFER_SIZE 1024
#define MAX_CONNECTS 9000

#define MAXCORES 16

struct ThreadRec {
  int tid; // 1, in; 0, none; 2, out
  int socket_on; // 1, on; 0, off
  pthread_t pid;
  int clientfd;
};

void *runthread(void *arg);

int server_sockfd;

int main(int argc, char *argv[]) {
  struct ThreadRec pthpool[MAXCORES]; // threads
  int flag;

  struct sockaddr_in server_sockaddr; // server's address information
  struct sockaddr client_sockaddr;

  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(SERV_PORT);
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(server_sockaddr.sin_zero), 8);


  server_sockfd  = socket(AF_INET, SOCK_STREAM, 0);
  bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr));
  listen(server_sockfd, LEN_QUEUE);

  flag = fcntl(server_sockfd, F_SETFL, 0);
  fcntl(server_sockfd, F_GETFL, flag | O_NONBLOCK); // set to non-blocking mode

  printf("--------before create threads\n"); /////////////////////
  for (int tmpi = 0; tmpi < MAXCORES; tmpi++) {
    pthpool[tmpi].tid = tmpi; pthpool[tmpi].socket_on = 0; // init
    printf("after init %d\n", tmpi); ///////////////////////
    pthread_create(&pthpool[tmpi].pid, NULL, runthread, (void *)&pthpool[tmpi]);
  }
}

void *runthread(void *arg) {
  struct ThreadRec *rec = (struct ThreadRec *)arg; //
  struct sockaddr client_sockaddr;
  socklen_t len_sockaddr = sizeof(struct sockaddr);;
  int client_sockfd;
  int ret = 999;
  char recv_buff[1024];
  int flag;

  struct epoll_event ev, events[MAX_CONNECTS]; // max connects
  int efd;
  efd = epoll_create(MAX_CONNECTS);
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = server_sockfd;
  epoll_ctl(efd, EPOLL_CTL_ADD, server_sockfd, &ev);
  printf("--------before create threads\n"); /////////////////////

  rec->socket_on = 0;

  while (1) {

    int num_events = epoll_wait(efd, events, MAX_CONNECTS, -1);

    for (int tmpi = 0; tmpi < num_events; tmpi++) { // handle every events
      if (events[tmpi].data.fd == server_sockfd) { // new connection in
        client_sockfd = accept(server_sockfd, &client_sockaddr, &len_sockaddr);
        if (client_sockfd > 0) {
          flag = fcntl(client_sockfd, F_SETFL, 0);
          fcntl(client_sockfd, F_GETFL, flag | O_NONBLOCK);

          ev.data.fd = client_sockfd;
          ev.events = EPOLLIN | EPOLLET;
          epoll_ctl(efd, EPOLL_CTL_ADD, client_sockfd, &ev);
        }

      }
      else if (events[tmpi].events & EPOLLIN){ //

        if (events[tmpi].data.fd < 0) continue;
        printf("##------------find a thread to handle. \n"); //////////////////

        memset(recv_buff, 0, sizeof(recv_buff));
        int num_recv = 0;
        while ((ret = read(events[tmpi].data.fd, recv_buff + num_recv, 1024)) > 0) num_recv += ret;

        if (ret == -1 && errno != EAGAIN) {
          printf("error recv data\n");
          close(events[tmpi].data.fd);
          events[tmpi].data.fd = -1;
        }

        if (ret == 0) {
          printf("ret == 0?\n");
          close(events[tmpi].data.fd);
          continue;
        }
        printf("[%d]recv: %s\n", rec->tid, recv_buff);



      } // end else
    }

  } // end while (1)
}
