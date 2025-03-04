#include<stdio.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>

struct message 
{
    char msg[1024];
    char time[100];
    int ct;
};
int main()
{
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    printf("CLIENT_FD: %d",client_fd);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(8080);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    if(connect(client_fd,(struct sockaddr*)&addr,sizeof(addr))<0)
    {
       perror("CONNECTION FAILED");
       exit(EXIT_FAILURE);
    }
    struct message receiver;
    send(client_fd,"HI FROM CLIENT",15,0);
    printf("MESSAGE SENT\n");
    recv(client_fd,&receiver,sizeof(receiver),0);
    printf("ECHO MESSAGE: %s\n",receiver.msg);
    printf("TIMESTAMP: %s\n",receiver.time);
    printf("COUNT:%d\n",receiver.ct);
}