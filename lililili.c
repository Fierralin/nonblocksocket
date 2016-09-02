#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>
#include<pthread.h>
#include<fcntl.h>

#define __DEBUG
#ifdef __DEBUG
#define DBG(fmt,args...) fprintf(stdout,  fmt,  ##args)
#else
#define DBG(fmt,args...)
#endif
#define ERR(fmt,args...) fprintf(stderr,  fmt,  ##args)

/*
isStreamQuit == 0 :Start
isStreamQuit == 1 :Client Quit
isStreamQuit ==-1 :App Quit
*/
static int isStreamQuit    = 0;
static int gStreamServerFd = -1;
int gStreamClientFd;

#define PORT_SOCKET          9000	//server监听端口
#define RECV_BUF_MAX_LEN     512
#define TIME_WAIT            200000		/*200ms*/

/*
socket发送函数
 */
int ScketSend(unsigned char *buf,unsigned int len)
{

	int ret = 0;
	if(gStreamClientFd <= 0)
		return -1;
	ret = send(gStreamClientFd,buf,len,0);
	if(ret <= 0 ){
		return -1;
	}else if(ret == len){
		return 0;
	}
	while(ret < len){
		ret += send(gStreamClientFd,buf+ret,len-ret,0);
	}
	return 0;
}

/*
socket连接线程，阻塞等待客户端的连接，
连接后循环处理客户端发来的数据，
这里只能连接一个客户端，稍加修改，就可以支持多客户端连接
*/
void *SocketThread(void *arg)
{
	fd_set SockRead;
	struct timeval TimeOut;
	int ret;
	struct sockaddr clientAddr;
	unsigned char *rcvBuf = (unsigned char *)malloc(RECV_BUF_MAX_LEN);

	socklen_t  len;
	len = sizeof(clientAddr);
	while(1){

		int rcvLen;
		/*阻塞等待客户端连接*/
		gStreamClientFd = accept(gStreamServerFd, &clientAddr, &len);
		if(gStreamClientFd == -1){
			ERR("ERROR in stream accept: %s\n",strerror(errno));
			/*add errno handler here,in some case,the error is not fatal*/
			/*code here*/
			break;
		}
		/*isStreamQuit == -1,is the siganl to quit*/
		if(isStreamQuit != -1){
			isStreamQuit = 0;
		}
		DBG("accep a stream client\n");
		/*客户端已连接，循环处理*/
		while(isStreamQuit==0){
			FD_ZERO(&SockRead);
			FD_SET(gStreamClientFd,&SockRead);
			TimeOut.tv_sec = 0;
			TimeOut.tv_usec = TIME_WAIT;
			/*
			200ms超时判断是否有数据接收，有则接收，没有继续循环，
			总是让程序不能一直阻塞在这里
			*/
			ret = select(gStreamClientFd+1,&SockRead,NULL,NULL,&TimeOut);
			if(ret == -1){
				ERR("ERROR in select\n");
				break;
			}else if(ret == 0){
				continue;
			}
			/*接收客户端发来的数据*/
			rcvLen = recv(gStreamClientFd,rcvBuf,RECV_BUF_MAX_LEN,0);
			if(rcvLen < 0){
				ERR("ERROR in stream accept: %s\n",strerror(errno));
				/*add errno handler here,in some case,the error is not fatal*/
				/*code here*/
				isStreamQuit = 1;
				break;
			}else if(rcvLen == 0){
				DBG("stream client close\n");
				/*code here*/
				isStreamQuit = 1;
				break;
			}
			DBG("rcvLen: %d,rcvBuf: %s\n",rcvLen,rcvBuf);

			memset(rcvBuf,0,RECV_BUF_MAX_LEN);
		}
		DBG("Close Stream Socket\n");
		close(gStreamClientFd);

		/*OTHER Error or App Quit*/
		if(isStreamQuit == -1){
			break;
		}

	}
	DBG("SocketThread exit\n");
	free(rcvBuf);
	close(gStreamServerFd);
	pthread_exit(NULL);
}

/*
socket连接初始化
*/
int SocketInit()
{
	struct sockaddr_in server;
	pthread_t tStreamSocketThr;
	/*创建socket*/
	gStreamServerFd = socket(AF_INET,SOCK_STREAM,0);
	if(gStreamServerFd == -1 ){
		ERR("ERROR in create Stream Socket!\n");
		return -1;
	}
	memset(&server,0,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT_SOCKET);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	/*绑定监听端口*/
	if(bind(gStreamServerFd ,(struct sockaddr*)&server,sizeof(server))== -1){
		ERR("ERROR in bind Stream Socket: %s\n",strerror(errno));
		return -1;
	}
	/*开始监听*/
	if(	listen(gStreamServerFd, 5) == -1){
		ERR("ERROR in listen Stream Socket: %s\n",strerror(errno));
		return -1;
	}
	/*创建socket连接线程，用来处理监听的结果，比如有客户端连接等*/
	pthread_create(&tStreamSocketThr,NULL,SocketThread,NULL);
	/*release thread resource,so we not use pthread_join to waitting*/
	pthread_detach(tStreamSocketThr);
	return 0;
}

#if 1
int main()
{
	if(SocketInit() == -1){
		return -1;
	}
	while(1){
		sleep(2);
	}

	/*notice to exit*/
	isStreamQuit = -1;
	sleep(1);
	if(gStreamServerFd){
		close(gStreamServerFd);
	}
	DBG("SUCCESS in ConnectManageExit\n");
}
#endif
