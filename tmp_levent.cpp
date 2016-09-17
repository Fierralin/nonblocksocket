#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>       // pthread_t , pthread_attr_t and so on.
#include <stdio.h>
#include <netinet/in.h>      // structure sockaddr_in
#include <arpa/inet.h>       // Func : htonl; htons; ntohl; ntohs
#include <assert.h>          // Func :assert
#include <string.h>          // Func :memset
#include <unistd.h>          // Func :close,write,read
#include <fcntl.h>
#include <event.h>
#define SOCK_PORT 6339
#define BUFFER_LENGTH 1024
#define MAX_CONN_LIMIT 512     // MAX connection limit

static void read_buffer(int client_socket_fd, short event_type, void *arg);

void run_thread(void *arg);

int main() {

}


int set_nonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
        return -1; // F_GETFD or F_GETFL ??????
    return 0;
}

void run_thread(void *arg) {
    struct event listen_ev;
    struct event_base *base = event_base_new();

    int client_socket_fd = (int) arg;
    if (client_socket_fd < 0) { //
        fprintf(stderr, "client socket fd < 0\n");
        return;
    }

    if (!set_nonblocking(client_socket_fd)) {
        fprintf(stderr, "set nonblocking failed\n");
        close(client_socket_fd); return;
    }

    while (1) {
        event_set(&listen_ev, client_socket_fd, EV_READ, read_buffer, (void *)ttt);
        if (event_base_set(base, listen_ev) != 0) {
            fprintf(stderr, "event base set is not 0\n");
            close(client_socket_fd); break;
        }
        event_base_loop(base, EVLOOP_NONBLOCK);
    }
    return;
}
