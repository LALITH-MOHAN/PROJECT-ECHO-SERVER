#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct message 
{
    char msg[1024];
    char time[10];
    char date[11];
    int ct;
    int uptime;  
};

void encrp_decrp(char *msg, char *key);

int main()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("CLIENT_FD: %d\n", client_fd);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
       perror("CONNECTION FAILED");
       exit(EXIT_FAILURE);
    }
    
    struct message receiver;
    #define KEY "key"
    char buff[] = "HI FROM CLIENT";
    encrp_decrp(buff, KEY);
    send(client_fd, buff, sizeof(buff), 0);
    printf("MESSAGE SENT\n");
    
    recv(client_fd, &receiver, sizeof(receiver), 0);
    encrp_decrp(receiver.msg, KEY);
    
    printf("ECHO MESSAGE: %s\n", receiver.msg);
    printf("DATE: %s\n", receiver.date);
    printf("TIMESTAMP: %s\n", receiver.time);
    printf("COUNT: %d\n", receiver.ct);
    printf("UPTIME: %d seconds\n", receiver.uptime);  
    printf("\n");
    sleep(5);
    close(client_fd);
}

void encrp_decrp(char *msg, char *key)
{
    int keyl = strlen(key);
    for (int i = 0; msg[i] != '\0'; i++)
    {
       msg[i] ^= key[i % keyl];
    }
}
