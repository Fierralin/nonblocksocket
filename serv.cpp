#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

#define MAXLINE 80
#define LISTENQ 20
#define SERV_PORT 6339

int SetNonBlock(int iSock) {
  int iFlags;

  iFlags = fcntl(iSock, F_GETFL, 0);
  iFlags |= O_NONBLOCK;
  iFlags |= O_NDELAY;
  int ret = fcntl(iSock, F_SETFL, iFlags);
  return ret;
}

int WriteNonBlock(int fd_, const char *send_buf, size_t send_len) {
  int write_pos = 0; // 发送位置
  int nLeft = send_len; // 表示未发完的数据
  while (nLeft > 0) {
	int nWrite = write(fd_, send_buf + write_pos, nLeft); // 已发完的数据长度
	if (nWrite == 0) return -1;
	if (nWrite < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN) nWrite = 0;
		else if (errno == EINTR) continue;
		else return -1;
	}
	else {
		nLeft -= nWrite;
		write_pos += nWrite;
	}
  }
  return 0;
}

int ReadNonBlock(int fd_, char* recv_buf, size_t recv_len) {
  while (1)   {
	int ret = read(fd_, recv_buf, recv_len);
	if (ret > 0) return 0;
	if (ret == 0) return -1;
	if (errno == EAGAIN || errno == EWOULDBLOCK
	        || errno == EINTR)
		continue;
  }
  return -1;
}

int main(int argc, char* argv[]) {
  int tmpi, listenfd, connfd, epfd, nfds, portnumber = SERV_PORT;
  ssize_t num = -1;
  char line[MAXLINE];
  socklen_t clilen;

  struct sockaddr_in clientaddr;
  struct sockaddr_in serveraddr;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd <= 0) {
	cout << "create fd fail" << endl;
	return -1;
  }
  SetNonBlock(listenfd);

  struct epoll_event ev, events[20];
  epfd = epoll_create(256);
  ev.data.fd = listenfd;
  ev.events = EPOLLIN | EPOLLET;

  epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

  bzero(&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  char local_addr[20] = "127.0.0.1";
  inet_aton(local_addr, &(serveraddr.sin_addr));
  serveraddr.sin_port = htons(portnumber);

  bind(listenfd, (sockaddr *)&serveraddr, sizeof(serveraddr));
  listen(listenfd, LISTENQ);

  cout << "server listening ..."  << endl;

  int ret = -1;

  while (true) {
	nfds = epoll_wait(epfd, events, 20, -1); // nfds 需要处理的请求数目

	for (tmpi = 0; tmpi < nfds; ++tmpi) {
		if (events[tmpi].data.fd == listenfd) { // 如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接
			connfd = accept(listenfd, (sockaddr *)&clientaddr, &clilen); // 以后读写都用这个返回的fd
			if (connfd < 0) {
				perror("connfd<0,accept fail"); exit(1);
			}
			SetNonBlock(connfd);//设置为非阻塞模式
			char *str = inet_ntoa(clientaddr.sin_addr);
			cout << "accapt a connection from " << str << endl;

			ev.data.fd = connfd;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
		}
		else if (events[tmpi].events & EPOLLIN) { // 如果是已经连接的用户，并且收到数据，那么进行读入
			connfd = events[tmpi].data.fd;

			ret = ReadNonBlock(connfd, line, MAXLINE);
			if (ret != 0) {
				cout << "read line fail" << endl;
				close(connfd);
				break;
			}
			cout << "receive:" << line << endl;

			ev.data.fd = connfd;
			ev.events = EPOLLOUT | EPOLLET;
			epoll_ctl(epfd, EPOLL_CTL_MOD, connfd, &ev); // 修改标识符，等待下一个循环时发送数据，异步处理的精髓
		}
		else if (events[tmpi].events & EPOLLOUT) {
			connfd = events[tmpi].data.fd;

			for (int tmpj = 0; tmpj < num; tmpj++)
				line[tmpj] = toupper(line[tmpj]);

			ret = WriteNonBlock(connfd, line, MAXLINE);
			if (ret != 0) {
				cout << "send line fail" << endl;
				close(connfd);
				break;
			}
			cout << "send:" << line << endl;

			ev.data.fd = connfd;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(epfd, EPOLL_CTL_MOD, connfd, &ev);
		}
	} // end for-loop
  } // end while (true)
  return 0;
}
