

#define PORT 6339
#define BACKLOG 5
#define MEM_SIZE 1024

struct event_base* base;

int main(int argc, char* argv[]) {
    struct sockaddr_in my_addr;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));
    listen(sock, BACKLOG);

    struct event listen_ev;
    base = event_base_new();
    event_set(&listen_ev, sock, EV_READ|EV_PERSIST, on_accept, NULL);
    event_base_set(base, &listen_ev);
    event_add(&listen_ev, NULL);
    event_base_dispatch(base);

    return 0;
}

void on_accept(int sock, short event, void* arg)
{
    struct sockaddr_in cli_addr;
    int newfd, sin_size;
    // read_ev must allocate from heap memory, otherwise the program would crash from segmant fault
    struct event* read_ev = (struct event*)malloc(sizeof(struct event));
    sin_size = sizeof(struct sockaddr_in);
    newfd = accept(sock, (struct sockaddr*)&cli_addr, &sin_size);
    event_set(read_ev, newfd, EV_READ|EV_PERSIST, on_read, read_ev);
    event_base_set(base, read_ev);
    event_add(read_ev, NULL);
}

void on_read(int sock, short event, void* arg)
{
    struct event* write_ev;
    int size;
    char* buffer = (char*)malloc(MEM_SIZE);
    bzero(buffer, MEM_SIZE);
    size = recv(sock, buffer, MEM_SIZE, 0);
    printf("receive data:%s, size:%d\n", buffer, size);
    if (size == 0) {
        event_del((struct event*)arg);
        free((struct event*)arg);
        close(sock);
        return;
    }
    write_ev = (struct event*) malloc(sizeof(struct event));;
    event_set(write_ev, sock, EV_WRITE, on_write, buffer);
    event_base_set(base, write_ev);
    event_add(write_ev, NULL);
}

void on_write(int sock, short event, void* arg) {
    char* buffer = (char*)arg;
    send(sock, buffer, strlen(buffer), 0);

    free(buffer);
}
