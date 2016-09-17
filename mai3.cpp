#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>

#include <event.h>


#define LISTENQ 20
#define SERV_PORT 6339

void doaccept(int sock, short event, void *arg);
void runthread(int sock);
void thread_process(void *arg);

int main(void) {
  int listenfd;
  struct sockaddr_in ser_addr;
  struct sockaddr_in cli_addr;
  socklen_t sin_size;

  ser_addr.sin_family = PF_INET;
  ser_addr.sin_port = SERV_PORT;
  ser_addr.sin_addr.s_addr = INADDR_ANY;

  listenfd  = socket(AF_INET, SOCK_STREAM, 0);
  bind(listenfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
  listen(listenfd, 2);

  // libevent main process here
  struct event_base *base = event_base_new();
  struct event listen_ev;
  event_set(&listen_ev, listenfd, EV_READ|EV_PERSIST, doaccept, NULL);
  event_base_set(base, &listen_ev);
  event_add(&listen_ev, NULL);
  event_base_dispatch(base);

  // always not run
  event_del(&listen_ev);
  event_base_free(base);

  // only reference --------------- no use here
  int connectfd = accept(listenfd, (struct sockaddr *)&cli_addr, &sin_size);
  char sendbuff[] = "ahahaha";
  send(connectfd, sendbuff, strlen(sendbuff), 0);
}

void doaccept(int sock, short event, void *arg)  {
  struct sockaddr_in remote_addr;
  int sinsize = sizeof(struct sockaddr_in);
  int remotefd = accept(sock, (struct sockaddr *)&remote_addr, (socklen_t *)&sinsize);
  runthread(remotefd); // create new thread to handle fd
}

void runthread(int sock) {
  pthread_t thread;
  pthread_create(&thread, NULL, ,(void *)sock);
  pthread_detach(thread);
}
