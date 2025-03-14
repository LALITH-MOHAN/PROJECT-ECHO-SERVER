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
    char msg[1024];  //message
    char time[10];  //time stamp
    char date[11]; //present date
    int ct;       //count for no of messages echoed
    int uptime;  //uptime in seconds
};

void encrp_decrp(char *msg, char *key); //encrypt & decrypt

int main() 
{
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0); // socket creation
    if (server_fd == -1) 
    {
        perror("SOCKET CREATION FAILED");
        exit(EXIT_FAILURE);
    }
    printf("SERVER SOCKET CREATED FD: %d\n", server_fd);

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) // bind the server socket
    {
        perror("BINDING FAILED");
        exit(EXIT_FAILURE);
    }
    printf("SERVER BOUND TO PORT %d\n", PORT);

    int total_messages = 0; // message count
    time_t start_time = time(NULL); // start of server.

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
        time_info = localtime(&current_time); // current time
        sprintf(det.time, "%02d:%02d:%02d", time_info->tm_hour, time_info->tm_min, time_info->tm_sec); // current timestamp
        sprintf(det.date, "%d/%d/%d", time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_year + 1900); // present date
        
        #define KEY "key" //secret key
        encrp_decrp(buff, KEY); // decrypt the message
        strncpy(det.msg, buff, sizeof(det.msg) - 1);
        det.msg[sizeof(det.msg) - 1] = '\0';
        total_messages++;

        det.ct = htonl(total_messages); 
        det.uptime = htonl((int)(current_time - start_time));

        encrp_decrp(det.msg, KEY); // encrypt the message before echoing
        sendto(server_fd, (char*)&det, sizeof(det), 0, (struct sockaddr*)&client_addr, client_len);
        
        encrp_decrp(det.msg, KEY);

        printf("MESSAGE SENT: %s at %s on %s, Uptime: %d seconds\n", det.msg, det.time, det.date, ntohl(det.uptime));
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
