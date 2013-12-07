#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define ERR_EXIT(m) \
	do{\
		perror (m);\
		exit(EXIT_FAILURE);\
	}while(0)


int main(int argc,char* argv[])
{
	int i = 0;
	char hostname[32] = {0};
	char username[32] = {0};
	char password[32] = {0};

	for(i=1;i<argc;i+=2)
	{
		if(!((strcmp(argv[i],"-h")== 0) ||(strcmp(argv[i],"-p")==0)||(strcmp(argv[i],"-u")==0)))
		{
			fprintf(stderr,"h3clogn:Invaild option--'%s'\nUseage: h3clogin [hup]\n-h\t\thostname\n-u\t\tusername\n-p\t\tpassword\n",argv[i]);
			return -1;
		}
	}

	for(i=1;i<argc;i++)
	{
		if(strcmp(argv[i],"-h")==0)
			strcpy(hostname,argv[i+1]);
		if(strcmp(argv[i],"-u")==0)
			strcpy(username,argv[i+1]);
		if(strcmp(argv[i],"-p")==0)
			strcpy(password,argv[i+1]);
	}

	int sockfd,ret;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
		ERR_EXIT("socket error.");

	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(23);
	servaddr.sin_addr.s_addr = inet_addr(hostname);
	
	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0)
		ERR_EXIT("connect error");
	
	char recvBuf[8096] = {0};
	char sendBuf[1024] = {0};

	i = strlen(username);
	username[i+1] = '\n';
	sleep(1);
	ret = write(sockfd,username,sizeof(username));
	if(ret == -1)
		ERR_EXIT("write error.");
	
	i = strlen(password);
	password[i+1] = '\n';
	sleep(1);
	ret = write(sockfd,password,sizeof(password));
	if(ret == -1)
		ERR_EXIT("write error.");

	pid_t pid;
	pid = fork();
	if(pid == -1)
	{
		ERR_EXIT("fork error.");
	}
	else if(pid == 0)
	{
		while(1)
		{
			memset(sendBuf,0,sizeof(sendBuf));
			fgets(sendBuf,sizeof(sendBuf),stdin);
			if(strlen(sendBuf) > 0)
				write(sockfd,sendBuf,strlen(sendBuf)+1);	
		}
	}else{
		while(1)
		{
			memset(recvBuf,0,sizeof(recvBuf));
			ret = read(sockfd,recvBuf,sizeof(recvBuf));
			if(ret == -1)
			{
				ERR_EXIT("read error");
			}
			fprintf(stderr,"%s",recvBuf);
		}
	}
	
	close(sockfd);
	return 0;
}
