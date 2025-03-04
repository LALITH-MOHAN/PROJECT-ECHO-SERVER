#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#define PORT 8080

struct message 
{
    char msg[1024];
    char time[100];
    int ct;
};

int main() 
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("SOCKET CREATION FAILED");
        exit(EXIT_FAILURE);
    }
    printf("SERVER FD: %d\n", server_fd);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int optval=1;
    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("BINDING FAILED");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) 
    {
        perror("LISTENING FAILED");
        exit(EXIT_FAILURE);
    }

    printf("LISTENING...\n");
    int client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_sock < 0) 
    {
        perror("ACCEPT FAILED");
        exit(EXIT_FAILURE);
    }

    struct message det;
    char buff[1024] = {0};

    int rec = recv(client_sock, buff, sizeof(buff) - 1, 0);
    if (rec < 0) {
        perror("RECEIVE FAILED");
        exit(EXIT_FAILURE);
    }
    buff[rec] = '\0'; 

    // Get current time
    time_t current_time;
    struct tm *time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    sprintf(det.time, "%d:%d:%d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
    strncpy(det.msg, buff, sizeof(det.msg) - 1);
    det.msg[sizeof(det.msg) - 1] = '\0'; 
    det.ct = 1;
    send(client_sock, (char*)&det, sizeof(det), 0);
    printf("MESSAGE SENT: %s at %s\n", det.msg, det.time);
    close(client_sock);
    close(server_fd);
    
    return 0;
}
