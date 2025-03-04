#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

struct Testclass
{
    char name[32];
    char saddr[250];
    int regno;
};

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }
    printf("SERVER_FD: %d\n", server_fd);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        return 1;
    }
    printf("LISTENING...\n");

    struct Testclass p;
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) 
    {
        perror("Accept failed");
        return 1;
    }

    recv(client_fd, &p, sizeof(struct Testclass), 0);
    printf("NAME: %s\n", p.name);
    printf("ADDRESS: %s\n", p.saddr);
    printf("NO: %d\n", p.regno);

    send(client_fd, "RECEIVED", 8, 0);
    close(client_fd);
    close(server_fd);
}
