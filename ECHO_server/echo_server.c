#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "message.h"
#include "encrypt.h"

#define PORT 8080

int main() 
{
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) 
    {
        perror("SOCKET CREATION FAILED");
        exit(EXIT_FAILURE);
    }
    printf("SERVER SOCKET CREATED FD: %d\n", server_fd);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("BINDING FAILED");
        exit(EXIT_FAILURE);
    }
    printf("SERVER BOUND TO PORT %d\n", PORT);

    int total_messages = 0;
    time_t start_time = time(NULL);

    while (1) 
    {
        struct message det;
        char buff[1024] = {0};
        
        int rec = recvfrom(server_fd, buff, sizeof(buff) - 1, 0, (struct sockaddr*)&client_addr, &client_len);
        if (rec < 0) 
        {
            perror("RECEIVING FAILED");
            continue;
        }
        
        buff[rec] = '\0';
        time_t current_time;
        struct tm *time_info;
        time(&current_time);
        time_info = localtime(&current_time);
        sprintf(det.time, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec);
        sprintf(det.date, "%d/%d/%d", time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900);
        
        #define KEY "key"
        encrp_decrp(buff, KEY);
        strncpy(det.msg, buff, sizeof(det.msg) - 1);
        det.msg[sizeof(det.msg) - 1] = '\0';
        total_messages++;

        det.ct = htonl(total_messages); 
        det.uptime = htonl((int)(current_time - start_time));

        encrp_decrp(det.msg, KEY);
        sendto(server_fd, (char*)&det, sizeof(det), 0, (struct sockaddr*)&client_addr, client_len);
        
        encrp_decrp(det.msg, KEY);

        printf("MESSAGE SENT: %s at %s on %s, Uptime: %d seconds\n", det.msg, det.time, det.date, ntohl(det.uptime));
    }
    printf("SERVER CLOSED\n");
    close(server_fd);
    return 0;
}
