#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

struct message 
{
    char msg[1024];  // message
    char time[10];   // time stamp
    char date[11];   // present date
    int ct;          // count for number of messages echoed
    int uptime;      // uptime in seconds
};

void encrp_decrp(char *msg, char *key);

int main()
{
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0); // socket for client
    if (client_fd < 0) 
    {
        perror("SOCKET CREATION FAILED");
        exit(EXIT_FAILURE);
    }
    printf("CLIENT SOCKET CREATED: FD %d\n", client_fd);

    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    struct message receiver;
    #define KEY "key" // secret key
    char buff[] = "HI FROM CLIENT";
    
    // Encrypt before sending
    encrp_decrp(buff, KEY);
    
    // Send message to the server
    if (sendto(client_fd, buff, sizeof(buff), 0, (struct sockaddr*)&server_addr, addr_len) < 0)
    {
        perror("SEND FAILED");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    printf("MESSAGE SENT TO SERVER\n");
    
    // Receive message from server
    if (recvfrom(client_fd, &receiver, sizeof(receiver), 0, (struct sockaddr*)&server_addr, &addr_len) < 0)
    {
        perror("RECEIVE FAILED");
        close(client_fd);
        exit(EXIT_FAILURE);
    }
    
    // Convert network byte order to host byte order
    receiver.ct = ntohl(receiver.ct);
    receiver.uptime = ntohl(receiver.uptime);
    
    // Decrypt the received message
    encrp_decrp(receiver.msg, KEY);
    
    printf("ECHO MESSAGE: %s\n", receiver.msg);
    printf("DATE: %s\n", receiver.date);
    printf("TIMESTAMP: %s\n", receiver.time);
    printf("COUNT: %d\n", receiver.ct);
    printf("UPTIME: %d seconds\n", receiver.uptime);  
    printf("\n");
    
    close(client_fd);
    return 0;
}

void encrp_decrp(char *msg, char *key)
{
    int keyl = strlen(key);
    for (int i = 0; msg[i] != '\0'; i++)
    {
        msg[i] ^= key[i % keyl];
    }
}
