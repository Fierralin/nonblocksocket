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

int main(int argc, char *argv[]) {
  int num_cores, corecount;
  struct ThreadRec pthpool[MAXCORES]; // threads
  struct epoll_event ev, events[MAX_CONNECTS]; // max connects
  int efd;

  int server_sockfd;
  int client_sockfd;
  int flag;
  socklen_t len_sockaddr;
  struct sockaddr_in server_sockaddr; // server's address information
  struct sockaddr client_sockaddr;

  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(SERV_PORT);
  server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(server_sockaddr.sin_zero), 8);

  len_sockaddr = sizeof(struct sockaddr);

  server_sockfd  = socket(AF_INET, SOCK_STREAM, 0);
  bind(server_sockfd, (struct sockaddr *)&server_sockaddr, sizeof(struct sockaddr));
  listen(server_sockfd, LEN_QUEUE);

  flag = fcntl(server_sockfd, F_SETFL, 0);
  fcntl(server_sockfd, F_GETFL, flag | O_NONBLOCK); // set to non-blocking mode

  efd = epoll_create(MAX_CONNECTS);
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = server_sockfd;
  epoll_ctl(efd, EPOLL_CTL_ADD, server_sockfd, &ev);

  for (int tmpi = 0; tmpi < MAXCORES; tmpi++) {
    pthpool[tmpi].tid = tmpi; pthpool[tmpi].socket_on = 0; // init
    pthread_create(&pthpool[num_cores].pid, NULL, runthread, (void *)&pthpool[num_cores]);
  }

  while (1) { //
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
      else { //

        if (events[tmpi].data.fd < 0) continue;

        while (1) { // wait for an empty thread
          int tmpj;
          for (tmpj = 0; tmpj < MAXCORES; tmpj++) {
            if (pthpool[tmpj].socket_on == 0) {
              pthpool[tmpj].clientfd = events[tmpi].data.fd;
              pthpool[tmpj].socket_on = 1;
              break;
            }
          }
          if (tmpj < MAXCORES) break;
        }

      } // end else
    } // end for-loop handling every events
  } // end while (1)

}

void *runthread(void *arg) {
  struct ThreadRec *rec = (struct ThreadRec *)arg; //
  struct sockaddr client_sockaddr;
  int ret = 999;
  char recv_buff[1024];

  rec->socket_on = 0;

  while (1) {

    while (rec->socket_on == 1) {

      ret = recv(rec->clientfd, recv_buff, 1024, 0);

      if (ret < 0) {
        if (errno == EAGAIN) fprintf(stderr, "EAGAIN\n");
        else fprintf(stderr, "recv error\n");
        close(rec->clientfd);
        rec->socket_on = 0;
      }//*
      else if (ret == 0) { // close properly
        close(rec->clientfd);
        rec->socket_on = 0;
        printf("[%d]recv: %s\n", rec->tid, recv_buff);
      }
    }

  } // end while (1)
}
