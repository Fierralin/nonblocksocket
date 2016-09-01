#include	"unp.h"

int
main(int argc, char **argv)
{
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				line[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	    if (argc != 2) {
			printf ("Usage %s <port>\n", argv[0]);
			exit(-1);
		}

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for ( ; ; ) {
		rset = allset;
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);
		        printf("Selected..\n");

		if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
			printf("listening socket readable\n");
			printf("sleeping 5 \n");
			sleep(5);
			clilen = sizeof(cliaddr);
			if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) <0) {
				            perror("accept error");
			            }

			printf("Accepted\n");

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;	/* save descriptor */
					                    printf("Assigned to slot %d\n", i);
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many clients");

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				            printf(" Socket %d have data to read\n", i);
				if ( (n = read(sockfd, line, MAXLINE)) <= 0) {
					printf("Readline Error\n");
					     /* connection closed by client */
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				Writen(sockfd, line, n);

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
}
