#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define TIME 20000

struct message 
{
    char msg[1024];  //message
    char time[10];  //time stamp
    char date[11]; //present date
    int ct;       //count for no of messages echoed
    int uptime;  //uptime in seconds
};

void encrp_decrp(char *msg, char *key); // for encryptuion and decryption

int main() 
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) 
    {
        perror("SOCKET CREATION FAILED");
        exit(EXIT_FAILURE);
    }
    printf("SERVER FD: %d\n", server_fd);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("BINDING FAILED");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) 
    {
        perror("LISTENING FAILED");
        exit(EXIT_FAILURE);
    }
    
    printf("LISTENING...\n");

    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    
    for (int i = 1; i <= MAX_CLIENTS; i++) 
    {
        fds[i].fd = -1;
    }
    
    int total_messages = 0; 
    time_t start_time = time(NULL);  // Server start time
    
    while (1) 
    {
        int activity = poll(fds, MAX_CLIENTS + 1, TIME);
        if (activity < 0) 
        {
            perror("POLL FAILED");
            exit(EXIT_FAILURE);
        }
        else if(activity == 0)
        {
            printf("NO NEW CONNECTIONS FOR %d secs\n", TIME / 1000);
            break;
        }
        if (fds[0].revents & POLLIN) 
        {
            int client_sock = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (client_sock < 0) 
            {
                perror("ACCEPT FAILED");
                continue;
            }
            
            for (int i = 1; i <= MAX_CLIENTS; i++) 
            {
                if (fds[i].fd == -1) 
                {
                    fds[i].fd = client_sock;
                    fds[i].events = POLLIN;
                    printf("NEW CONNECTION: FD %d\n", client_sock);
                    break;
                }
            }
        }
        
        for (int i = 1; i <= MAX_CLIENTS; i++) 
        {
            if (fds[i].fd != -1 && (fds[i].revents & POLLIN)) 
            {
                struct message det;
                char buff[1024] = {0};
                int rec = recv(fds[i].fd, buff, sizeof(buff) - 1, 0);
                
                if (rec <= 0) 
                {
                    printf("CLIENT DISCONNECTED: FD %d\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    continue;
                }
                
                buff[rec] = '\0';
                time_t current_time;
                struct tm *time_info;
                time(&current_time); // noting the current time
                time_info = localtime(&current_time);
                sprintf(det.time, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
                sprintf(det.date, "%d/%d/%d", time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900);
                
                #define KEY "key"
                encrp_decrp(buff, KEY);
                strncpy(det.msg, buff, sizeof(det.msg) - 1);
                det.msg[sizeof(det.msg) - 1] = '\0';
                total_messages++;
                det.ct = total_messages;
                
                // Calculate uptime
                det.uptime = (int)(current_time - start_time);

                printf("MESSAGE FROM CLIENT FD %d: %s\n", fds[i].fd, det.msg);
                
                encrp_decrp(det.msg, KEY);
                send(fds[i].fd, (char*)&det, sizeof(det), 0);
                encrp_decrp(det.msg, KEY);
                printf("MESSAGE SENT: %s at %s on %s (Uptime: %d seconds)\n", det.msg, det.time, det.date, det.uptime);
            }
        }
    }
    printf("SERVER CLOSED\n");
    close(server_fd);
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
