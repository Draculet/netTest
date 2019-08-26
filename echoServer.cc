#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
 
#define SERVERPORT 9981
#define MAXBUFFER 256
 
int main(int argc, char** argv)
{
    int serverFd, connfd,ret;
    socklen_t len;
    struct sockaddr_in serveraddr,clientaddr;
    char readBuf[MAXBUFFER]={0};
    char ip[40]={0};
    serverFd=socket(AF_INET,SOCK_STREAM,0);//创建socket
    if(serverFd < 0)
    {
        printf("socket error:%s\n",strerror(errno));
        exit(-1);
    }
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(SERVERPORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret=bind(serverFd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));//绑定IP和端口
    if(ret!=0)
    {
        close(serverFd);
        printf("bind error:%s\n",strerror(errno));
        exit(-1);
    }
    ret=listen(serverFd,5);//监听
    if(ret!=0)
    {
       close(serverFd);
       printf("listen error:%s\n",strerror(errno));
       exit(-1);
    }
    len=sizeof(clientaddr);
    bzero(&clientaddr,sizeof(clientaddr));
    int count = 0;
    while (1)
    {
    /*    connfd = accept(serverFd, (struct sockaddr *) &clientaddr, &len);//接受客户端的连接
        printf("%s 连接到服务器 %d 端口号 %d\n",inet_ntop(AF_INET,&clientaddr.sin_addr,ip,sizeof(ip)), count, ntohs(clientaddr.sin_port));
        if (connfd <= 0)
        {
            printf("accept error : %s\n", strerror(errno));
            sleep(5);
            continue;
        }
        else
        {
        	write(connfd,readBuf,MAXBUFFER);
        	count++;
        	if (count % 500 == 499)
        		sleep(5);
        }

        
        /*
        while((ret=read(connfd,readBuf,MAXBUFFER)))//读客户端发送的数据
        {
            write(connfd,readBuf,MAXBUFFER);//写回客户端
            printf("echo sucess\n");
            bzero(readBuf,MAXBUFFER);
        }
        if(ret==0)
        {
            printf("客户端关闭连接\n");         
        }else
        {
            printf("read error:%s\n",strerror(errno));
        }
        close(connfd);
        */
    }
    close(serverFd);
    return 0;
}
