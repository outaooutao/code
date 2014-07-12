#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#define SERVER_PORT 12345


void doRequest(int clientSocket)
{
	
	char buf[1024];
	char rbuf[1024];
	int n;
	
	while(1)
	{
		gets(buf);
		n=strlen(buf);
		if(send(clientSocket,buf,n+1,0)<0)
		{
			printf("send error\n");
			return;
		}
		printf("send ok\n");
		 n=recv(clientSocket,rbuf,1024,0);
		
		
		printf("server said:%s\n",rbuf);
	}
}



void main()
{
int clientSocket;
struct sockaddr_in serverAddr;


clientSocket=socket(AF_INET,SOCK_STREAM,0);
if(clientSocket<0)
{
	printf("client socket error\n");
	return;
}
printf("client socket ok\n");
bzero(&serverAddr,sizeof(serverAddr));
serverAddr.sin_family=AF_INET;
serverAddr.sin_port=htons(SERVER_PORT);
if(inet_aton("192.168.1.184",&serverAddr.sin_addr)==0)
{
	printf("inet_aton error\n");
	return;
}
if(connect(clientSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0)
{
	printf("connect error\n");
	return;
}
    printf("connect ok\n");
	doRequest(clientSocket);
    close(clientSocket);
	
	
	
	
}